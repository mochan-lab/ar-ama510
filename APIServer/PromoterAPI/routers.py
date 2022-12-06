from typing import List
from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from . import schemas, cruds
from .sql import database

router = APIRouter()

def get_db():
    db = database.SessionLocal()
    try:
        yield db
    finally:
        db.close()

@router.get("/")
async def hello():
    return {"message": "hello world!"}

@router.post("/client/connect", response_model=schemas.ClientID)
def connect(clientmeta: schemas.ClientMeta, db: Session = Depends(get_db)):
    ret = cruds.create_client(db,clientmeta)
    newrserver = cruds.newclient_findset_rserver(db, ret.name)
    if newrserver == 0:
        return schemas.ClientID(
            status="CANNOT_FIND_RSERVER",
            client=ret.name
        )
    return schemas.ClientID(
        status=ret.status,
        client=ret.name
    )

@router.get("/client/{client_id}/lookingrserver", response_model=schemas.ClientID)
def continueLooking(client_id: str, db: Session = Depends(get_db)):
    newrserver = cruds.newclient_findset_rserver(db, client_id)
    if newrserver == 0:
        return schemas.ClientID(
            status="CANNOT_FIND_RSERVER",
            client=client_id
        )
    return schemas.ClientID(
        status="LOOKING_FOR_RSERVER",
        client=client_id
    )


@router.get("/client/{client_id}/waitingcallee", response_model=schemas.DMPeerID)
def waitingCallee(client_id: str, db: Session = Depends(get_db)):
    ret = cruds.get_callee(db, client_id)
    if ret is None:
        return schemas.DMPeerID(
            status="NOW_CHECKING",
            client=client_id,
            dpeerid="",
            mpeerid=""
        )
    return schemas.DMPeerID(
        status=ret.status,
        client=ret.name,
        dpeerid=ret.dpeerid,
        mpeerid=ret.mpeerid
    )

@router.post("/client/{client_id}/statusupdate", response_model=schemas.ClientID)
def statusUpdateOut(client_id: str, clientStatus: schemas.StatusBase, db: Session = Depends(get_db)):
    ret = cruds.update_client_status(db, client_id, clientStatus.status)
    return schemas.ClientID(
        status=ret.status,
        client=ret.name
    )


@router.get("/rserver/{rserver_id}/waitinguser", response_model=schemas.ClientInfo)
def waitingUser(rserver_id: int, db: Session = Depends(get_db)):
    ret = cruds.find_new_client(db, rserver_id)
    if ret is None:
        return schemas.ClientInfo(
            status="NONE",
            client="",
            rserver=rserver_id,
            dpeerid="",
            mpeerid="",
            clientwidth=0,
            clientheight=0
        )
    return ret
    

@router.post("/rserver/{rserver_id}/pleasecall", response_model=schemas.ClientID)
def pleaseCall(rserver_id: int, toclient: schemas.DMPeerID, db: Session = Depends(get_db)):
    ret = cruds.set_peerid_status(db, rserver_id, toclient)
    return schemas.ClientID(
        status=ret.status,
        client=ret.name,
        dpeerid=ret.dpeerid,
        mpeerid=ret.mpeerid
    )

@router.post("/rserver/{rserver_id}/statusupdate", response_model=schemas.ClientID)
def statusUpdateIn(rserver_id: int, clientid: schemas.ClientID, db: Session = Depends(get_db)):
    ret = cruds.get_client(db, clientid.client)
    if ret.rserver != rserver_id:
        raise HTTPException(status_code=400, detail="rserver is not valid")
    ret = cruds.update_client_status(db, clientid.client, clientid.status)
    return schemas.ClientID(
        status=ret.status,
        client=ret.name
    )

@router.get("/rserver/{rserver_id}/waitingquit/{client_id}", response_model=schemas.ClientID)
def waitingQuit(rserver_id: int, client_id: str, db: Session = Depends(get_db)):
    ret = cruds.find_client_quiting(db, rserver_id, client_id)
    if ret is None:
        return schemas.ClientID(
            status="NOT_EXIST_THE_CLIENT",
            client=client_id
        )
    if ret == 1:
        return schemas.ClientID(
            status="CLIENT_QUITED",
            client=client_id
        )
    return schemas.ClientID(
        status=ret.status,
        client=ret.name
    )

@router.post("/rserver/create", response_model=schemas.RserverWithClients)
def createRserver(rservermeta: schemas.RserverInfo, db: Session = Depends(get_db)):
    ret = cruds.create_rserver(db, rservermeta)
    return schemas.RserverWithClients(
        rserver=ret.rserverid,
        status=ret.status,
        priority=ret.priority,
        maxnum=ret.maxnum,
        nowusers=ret.nowusers
    )

@router.post("/rserver/{rserver_id}/rserverstatusupdate/{nowcount}", response_model=schemas.RserverWithClients)
def rserverStatusUpdate(rserver_id: int, nowcount: int, rservermeta: schemas.RserverID, db: Session = Depends(get_db)):
    ret = cruds.get_rserver(db, rserver_id)
    if ret is None:
        raise HTTPException(status_code=400, detail="rserver is not exist")
    ret = cruds.update_rserver_status(db, rserver_id, rservermeta.status, nowcount)
    return schemas.RserverWithClients(
        rserver=ret.rserverid,
        status=ret.status,
        priority=ret.priority,
        maxnum=ret.maxnum,
        nowusers=ret.nowusers
    )
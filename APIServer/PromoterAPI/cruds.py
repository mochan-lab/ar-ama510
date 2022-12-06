import random, string, datetime
from sqlalchemy import desc
from sqlalchemy.orm import Session
from . import schemas
from .sql.database import Client, Rserver

def randomname(n):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=n))

def get_client(db: Session, clientname: str):
    return db.query(Client).filter(Client.name == clientname).first()

def get_rserver(db: Session, rserverid: int):
    return db.query(Rserver).filter(Rserver.rserverid == rserverid).first()

def newclient_findset_rserver(db : Session, clientname: str):
    nowusers = 99999
    ret = 0
    client = get_client(db, clientname)
    if client.status != "LOOKING_FOR_RSERVER":
        return ret
    rservers = db.query(Rserver).order_by(Rserver.priority).all()
    for rserver in rservers:
        if rserver.status != "WAITING_NEW_CLIENT" or rserver.nowusers >= rserver.maxnum:
            continue
        if rserver.nowusers < nowusers:
            nowusers = rserver.nowusers
            ret = rserver.rserverid
    client.rserver = ret
    if ret != 0:
        # client.status = "CHECKING_CLIENT_CONNECT"
        rserver = get_rserver(db, ret)
        rserver.status = "ACCEPTING_CLIENT"
        # rserver.nowusers += 1
    client.updatedatetime=datetime.datetime.now()
    db.commit()
    return ret

def create_client(db: Session, clientmeta: schemas.ClientMeta):
    client_name = randomname(20)
    while get_client(db, client_name) is not None:
        client_name = randomname(20)
    db_client = Client(
        name=client_name,
        status="LOOKING_FOR_RSERVER",
        clientwidth=clientmeta.clientwidth,
        clientheight=clientmeta.clientheight,
        useragent=clientmeta.useragent,
        dpeerid=client_name+"dpeer",
        mpeerid=client_name+"mpeer",
        begindatetime=datetime.datetime.now(),
        updatedatetime=datetime.datetime.now()
    )
    db.add(db_client)
    db.commit()
    db.refresh(db_client)
    return db_client

def update_client_status(db: Session, clientid: str, newstatus: str):
    client=get_client(db, clientid)
    client.status = newstatus
    client.updatedatetime=datetime.datetime.now()
    db.commit()
    db.refresh(client)
    #client=get_client(db, clientid)
    return client

def set_peerid_status(db: Session, rserver: int, toclient: schemas.DMPeerID):
    client=get_client(db, toclient.client)
    client.dpeerid=toclient.dpeerid
    client.mpeerid=toclient.mpeerid
    client.rserver=rserver
    client.status=toclient.status
    client.updatedatetime=datetime.datetime.now()
    db.commit()
    db.refresh(client)
    # client=get_client(db, toclient.client)
    return client

# def set_rserver(db: Session, clientname: str, rserver: int, newstatus: str):
#     client=get_client(db, clientname)
#     client.rserver=rserver
#     client.status=newstatus
#     client.updatedatetime=datetime.datetime.now()
#     db.commit()
#     db.refresh(client)
#     # client=get_client(db, clientname)
#     return client

def find_new_client(db: Session, rserverid: int):
    rserver=get_rserver(db, rserverid)
    if(rserver.status != "ACCEPTING_CLIENT"):
        return None
    rserver.status = "CHECKING_CLIENT"
    client = db.query(Client).order_by(desc(Client.key)).filter(Client.rserver == rserverid, Client.status == "LOOKING_FOR_RSERVER").first()
    if client is None:
        rserver.status="WAITING_NEW_CLIENT"
        db.commit()
        return None
    client.status = "CHECKING_CLIENT_CONNECT"
    db.commit()
    db.refresh(client)
    return schemas.ClientInfo(
        status=client.status,
        client=client.name,
        rserver=client.rserver,
        dpeerid=client.dpeerid,
        mpeerid=client.mpeerid,
        clientwidth=client.clientwidth,
        clientheight=client.clientheight
    )

def get_callee(db: Session, clientname: str):
    client = get_client(db, clientname)
    client.updatedatetime=datetime.datetime.now()
    db.commit()
    db.refresh(client)
    if client.status != "SOON_CALL":
        return None
    return client

def create_rserver(db: Session, rservermeta: schemas.RserverInfo):
    db_rserver = Rserver(
        rserverid=rservermeta.rserver,
        status=rservermeta.status,
        priority=rservermeta.priority,
        maxnum=rservermeta.maxnum,
        nowusers=0
    )
    db.add(db_rserver)
    db.commit()
    db.refresh(db_rserver)
    return get_rserver(db, db_rserver.rserverid)

def update_rserver_status(db: Session, rserverid: int, newstatus: str, nowcount: int):
    rserver=get_rserver(db, rserverid)
    rserver.status = newstatus
    rserver.nowusers = nowcount
    db.commit()
    db.refresh(rserver)
    # rserver=get_rserver(db, rserverid)
    return rserver

def find_client_quiting(db: Session, rserverid: int, clientname:str):
    client = get_client(db, clientname)
    # if client is None:
    #     return None
    if client.status == "WANT_QUIT":
        client.status="CLIENT_QUITING"
        db.commit()
        db.refresh(client)
        return client
    elif client.status == "CLIENT_QUITING" or \
            client.status == "CLIENT_QUITED_FORM" or \
            client.status == "CLIENT_QUITED_TIMEOUT" or \
            client.status == "CLIENT_QUITED_INVALID":
        return 1
    else:
        # begintime = datetime.datetime.strptime(client.begindatetime, '%Y-%m-%d %H:%M:%S.%f')
        # lasttime = datetime.datetime.strptime(client.updatedatetime, '%Y-%m-%d %H:%M:%S.%f')
        lasttime = client.updatedatetime
        if datetime.datetime.now() - lasttime > datetime.timedelta(minutes=6):
            client.status = "CLIENT_QUITED_TIMEOUT"
            db.commit()
            db.refresh(client)
            return client
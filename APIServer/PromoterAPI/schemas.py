from typing import Optional
from pydantic import BaseModel, Field

class ClientMeta(BaseModel):
    clientwidth: int
    clientheight: int
    useragent: str

    class Config:
        orm_mode=True

class StatusBase(BaseModel):
    status: str = Field(...,description="return status")

    class Config:
        orm_mode=True

class ClientID(StatusBase):
    client: str

    class Config:
        orm_mode=True

class DMPeerID(ClientID):
    dpeerid: str
    mpeerid: str

    class Config:
        orm_mode=True

class ClientInfo(ClientID):
    rserver: int
    dpeerid: str
    mpeerid: str
    clientwidth: int
    clientheight: int

    class Config:
        orm_mode=True

class RserverID(StatusBase):
    rserver: int

    class Config:
        orm_mode=True

class RserverInfo(StatusBase):
    rserver: int
    priority: int
    maxnum: int

    class Config:
        orm_mode=True

class RserverWithClients(RserverInfo):
    nowusers: int

    class Config:
        orm_mode=True
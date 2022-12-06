from sqlalchemy import Boolean, Column, Integer, String, DateTime, create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

#データベースファイルの設定。SQLiteを利用。
SQLALCHEMY_DATABASE_URL = "sqlite:///./DatabaseName.db"

engine = create_engine(
    SQLALCHEMY_DATABASE_URL, connect_args={"check_same_thread": False}
)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

Base = declarative_base()

class Client(Base):
    __tablename__ = "clients"
    key = Column('key', Integer, primary_key = True)
    name = Column('name', String(100))
    status = Column('status', String(100))
    dpeerid = Column('dpeerid', String(100))
    mpeerid = Column('mpeerid', String(100))
    clientwidth = Column('clientwidth', Integer)
    clientheight = Column('clientheight', Integer)
    useragent = Column('useragent', String)
    begindatetime = Column('begindatetime',DateTime)
    updatedatetime = Column('updatedatetime',DateTime)
    rserver = Column('rserver', Integer)

class Rserver(Base):
    __tablename__ = "rservers"
    key = Column('key', Integer, primary_key = True)
    rserverid = Column('rserverid', Integer)
    priority = Column('priority', Integer)
    maxnum = Column('maxnum', Integer)
    nowusers = Column('nowusers', Integer)
    status = Column('status', String)
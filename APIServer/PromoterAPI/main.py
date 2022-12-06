from fastapi import FastAPI
from starlette.middleware.cors import CORSMiddleware
from . import routers
from .sql import database

database.Base.metadata.create_all(bind=database.engine)


app = FastAPI()

origins = [
    "http://ar-ama510.com",
    "https://ar-ama510.com",
    "http://ar-ama510.conohawing.com",
    "https://ar-ama510.conohawing.com"
]
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(routers.router)
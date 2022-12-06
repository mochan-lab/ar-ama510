from fastapi import FastAPI
from starlette.middleware.cors import CORSMiddleware
from . import routers
from .sql import database

#データベースをbind
database.Base.metadata.create_all(bind=database.engine)


app = FastAPI()

#Cross-Origin Resource Sharing（CORS）解決のため
origins = [
    "Clients_domain"
]
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(routers.router)
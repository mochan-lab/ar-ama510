bind = "127.0.0.1:8000"
worker_class = "uvicorn.workers.UvicornWorker"
workers=1
timeout=60
graceful_timeout=60
loglevel = "debug"
accesslog = None
access_log_format = '%(h)s %(l)s %(u)s %(t)s "%(r)s" %(s)s %(b)s "%(f)s" "%(a)s"'
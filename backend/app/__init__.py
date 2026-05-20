from flask import Flask

app = Flask(__name__)
app.config.from_object('backend.config')

from backend.app import routes  # noqa: E402, F401

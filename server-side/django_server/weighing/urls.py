from django.urls import path
from . import consumers

websocket_urlpatterns = [
    path(r'/wss/set-balance/1/', consumers.WeighingConsumer.as_asgi())
]
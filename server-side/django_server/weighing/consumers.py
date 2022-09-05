import json
import redis
from asgiref.sync import async_to_sync
from channels.generic.websocket import JsonWebsocketConsumer
from channels.generic.websocket import WebsocketConsumer

class WeighingConsumer(JsonWebsocketConsumer):
    def connect(self):
        self.room_group_name = 'django_redis_chat'
        # join the room group
        async_to_sync(self.channel_layer.group_add)(
            self.room_group_name,
            self.channel_name
        )   
        self.accept()


    def chat_message(self, event):
        message = event['message']
        # set in redis

        self.send(text_data=json.dumps({
            'type':'message',
            'message':message
        }))


    def receive(self, text_data):
        text_data_json = json.loads(text_data)
        message = text_data_json['message']
        # Send message to room group
        async_to_sync(self.channel_layer.group_send)(
            self.room_group_name,
            {
                'type':'chat_message',
                'message':message
            }
        )

    def disconnect(self, close_code):
        # Leave room group
        async_to_sync(self.channel_layer.group_discard)(
            self.room_group_name,
            self.channel_name
        )


class SetBalance(WebsocketConsumer):
    def connect(self):
        self.user = self.scope["user"]
        print(self.user)
        self.balance_id = self.scope['url_route']['kwargs']['balance_id']
        self.balance_group_name = f"set-balance-{self.balance_id}"
        self.balance_group_name_get = f"get-balance-{self.balance_id}"


        # Join room group
        async_to_sync(self.channel_layer.group_add)(
            self.balance_group_name,
            self.channel_name
        )
        
        self.accept()


    def disconnect(self, close_code):
        # Leave room group
        async_to_sync(self.channel_layer.group_discard)(
            self.balance_group_name,
            self.channel_name
        )

    def receive(self, text_data):
        print(text_data)
        self.user = self.scope["user"]
        try:
            text_data_json = json.loads(text_data)
            message = float(text_data_json['message'])
        except:
            return

        try:
            secret = text_data_json['secret']
        except:
            secret = ""

        per = False
        if self.user.is_authenticated:
            # check user per
            per = True
        elif secret == "s3cr3t":
            per = True

        if per:
            r = redis.Redis(host='localhost',password='Redis12341234', port=6379, db=0)
            r.set(f"balance-{self.balance_id}", message*1000)
            async_to_sync(self.channel_layer.group_send)(
                self.balance_group_name_get,
                {
                    'type': 'send_value',
                    'message': message
                }
            )

"""
Before using MQTT over websockets we have to change the mosquitto.conf file, by adding the following:

listener 9001
protocol websockets
"""

# Importing The Client Class
import paho.mqtt.client as mqtt
import time
import certifi

def on_publish(client,userdata,mid):   #create function for callback
   print("data published mid=",mid, "\n")
   pass

def on_subscribe(client, userdata, mid, granted_qos):   #create function for callback
   print("subscribed with qos",granted_qos, "\n")
   pass

def on_message(client, userdata, message):
    print("message received :"  ,str(message.payload.decode("utf-8")))

def on_disconnect(client, userdata, rc):
   print("client disconnected ok")

# Creating a Client Instance
client_name = "test0_user00"
client = mqtt.Client(client_name, transport='websockets')
client.tls_set(certifi.where()) 

client.on_publish = on_publish        #assign function to callback
client.on_subscribe = on_subscribe        #assign function to callback
client.on_message = on_message        #assign function to callback

# Connecting To a Broker
print("connecting to broker...")
client.connect("mqtt.eclipseprojects.io", port=443, keepalive=60, bind_address="") #i used this public broker for testing

# Start loop
client.loop_start()


# Subscribing To Topics
print("Subscribing to topic","balance/test")
client.subscribe("balance/test")

time.sleep(3)

# Publishing Messages
print("Publishing to topic","balance/test")
client.publish("balance/test","test weight")

time.sleep(3)


client.disconnect()




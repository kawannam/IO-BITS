import paho.mqtt.client as mqtt
import paho.mqtt.subscribe as subscribe
from listener.EventHandler import EventHandler


class MQTTClient:
    PRESS = "iobits/ButtonPress"
    REQUEST = "iobits/DataRequest"
    UNDO = "iobits/UndoButtonPress"
    NEW_FILE = "iobits/NewFileRequest"

    def __init__(self, host_name):
        self.server = host_name
        self.client = mqtt.Client()
        print("Connecting to {}...".format(host_name))
        self.client.connect(host_name)
        print("Connected to {}!".format(host_name))
        self.handler = EventHandler()

    def listen(self):
        subscribe.callback(self.on_message, [MQTTClient.PRESS, MQTTClient.REQUEST, MQTTClient.UNDO, MQTTClient.NEW_FILE], hostname=self.server)

    def on_message(self, cdata, userdata, msg):
        if msg.topic == MQTTClient.PRESS:
            print("Received PRESS event: {}".format(msg.payload))
            self.handler.button_press_event(msg.payload.decode("utf-8"))
        elif msg.topic == MQTTClient.REQUEST:
            print("Received REQUEST event: {}".format(msg.payload))
            self.handler.data_request_event(msg.payload.decode("utf-8"), self.server)
        elif msg.topic == MQTTClient.UNDO:
            print("Received UNDO event: {}".format(msg.payload))
            self.handler.undo_request_event(msg.payload.decode("utf-8"))
        elif msg.topic == MQTTClient.NEW_FILE:
            print("Received NEW_FILE event: {}".format(msg.payload))
            self.handler.newfile_request_event(msg.payload.decode("utf-8"))
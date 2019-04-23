import paho.mqtt.publish as publish
import os, sys
import datetime

class EventHandler:
    EVENT_NUM = 10
    P1_DEVICES = ["A", "B", "C", "D"]
    PARTICIPANTS = [P1_DEVICES]
    BUTTONS = ["A", "B"]
    META_BUTTONS = ["U", "N", "S"] #undo, new, switch
    RESPONSE = "iobits/DataResponse"
    RESPONSE_POINTS = "iobits/DataResponsePoints"

    def __init__(self):
        p = 0;
        if not os.path.exists("META_DATA") :
            os.makedirs("META_DATA")
        if not os.path.exists("DATA") :
            os.makedirs("DATA")
        for participant in EventHandler.PARTICIPANTS:
            for device in participant:
                p = p + 1
                try:
                    file = open("DATA/{}.csv".format(device), "x")
                    file.close()
                    meta_file = open("META_DATA/{}.csv".format(device), "x")
                    meta_file.close()
                except FileExistsError:
                    continue

    @staticmethod
    def last_n_points(name, file, n):
        ret = []
        lines = file.readlines()
        for line in lines[max(0, len(lines)-n):]:
            ret.append((EventHandler.RESPONSE_POINTS, "{},{}".format(name, line.rstrip()), 0, False))
        return ret


    def button_press_event(self, message):
        fields = message.split(",")
        timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        is_log = fields[1] in self.BUTTONS
        print("BUTTON PRESS EVENT")
        if is_log:
            print("LOG {} BUTTON PRESSED".format(fields[1]))
            with open("{}.csv".format(fields[0]), "a") as file:
                file.write("{},{}\n".format(fields[1], timestamp))
        else:
            with open("META/{}.csv".format(fields[0]), "a") as file:
                file.write("{},{}\n".format(fields[1], timestamp))
            if fields[1] == "U":
                print("UNDO BUTTON PRESSED")
                self.undo_request_event(message)
            elif fields[1] == "N":
                print("NEW FILE BUTTON PRESSED")
                self.newfile_request_event(message)
            elif fields[1] == "S":
                print("SWITCH BUTTON PRESSED")


    def count_button_presses(self, file):
        press_data = [0] * len(self.BUTTONS)

        for line in file:
            i = 0;
            for button in self.BUTTONS:
                if (button == line[0]):
                    press_data[i] += 1
                    continue
                i += 1
        return press_data

            
    def data_request_event(self, message, server):
        fields = message.split(",")
        timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        with open("{}.csv".format(fields[0]), "r") as file:
            press_data = self.count_button_presses(file)
        
        with open("{}.csv".format(fields[0]), "r") as file:
            pts = EventHandler.last_n_points(fields[0], file, EventHandler.EVENT_NUM)

        counts = ','.join(str(c) for c in press_data)
        test = ",".join([fields[0], counts,str(len(pts)), timestamp]).rstrip(",")
        publish.single(EventHandler.RESPONSE,
                       payload= test,
                       hostname=server)
        if len(pts) > 0:
            publish.multiple(pts, hostname=server)
        print("Done responding to data request.")


    def undo_request_event(self, message):
        print("UNDO REQUEST")
        try:
            fields = message.split(",")
            with open("{}.csv".format(fields[0]), "r+") as file:
                lines = file.readlines()
                last_line = lines[-1].split(",")
                lines = lines[:-1]
                file.truncate(0)
                file.writelines([item for item in lines])
        except FileExistsError:
            print("File was not restarted correctly")


                        
    def newfile_request_event(self, message):
        fields = message.split(",")
        ts = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
        os.rename("{}.csv".format(fields[0]), "{}_{}.csv".format(fields[0],ts))
        try:
            file = open("{}.csv".format(fields[0]), "x")
            file.close()
        except FileExistsError:
            print("File was not restarted correctly")
        print("NEWFILE REQUEST")


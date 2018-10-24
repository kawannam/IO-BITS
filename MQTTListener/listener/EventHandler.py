import paho.mqtt.publish as publish
import os, sys
import datetime

class EventHandler:
    EVENT_NUM = 100
    NAMES = ["A", "B", "C", "D"]
    BUTTONS = ["A", "B"]
    RESPONSE = "iobits/DataResponse"
    RESPONSE_POINTS = "iobits/DataResponsePoints"

    def __init__(self):

        for name in EventHandler.NAMES:
            try:
                file = open("{}.csv".format(name), "x")
                file.close()
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
        with open("{}.csv".format(fields[0]), "a") as file:
            file.write("{}\n".format(",".join(fields[1:])))


    def count_button_presses(self, file):
        press_data = [BUTTONS.len()]
        for line in file:
            i = 0;
            for button in BUTTONS:
                if (button == line[0]):
                    press_data[i] += 0
                    continue
                i += 1
        return press_data

    def data_request_event(self, message, server):
        fields = message.split(",")
        with open("{}.csv".format(fields[0]), "r") as file:
            pts = EventHandler.last_n_points(fields[0], file, EventHandler.EVENT_NUM)
            counts = count_button_presses(file)
            publish.single(EventHandler.RESPONSE, payload=",".join([fields[0], counts,
                                                                    str(len(pts)).zfill(3)]).rstrip(","),
                           hostname=server)
            if len(pts) > 0:
                publish.multiple(pts, hostname=server)
            print("Done responding to data request.")

    def undo_request_event(self, message):
        fields = message.split(",")
        with open("{}.csv".format(fields[0]), "r+") as file:
            lines = file.readlines()
            last_line = lines[-1].split(",")
            lines = lines[:-1]
            file.writelines([item for item in lines])

                        
    def newfile_request_event(self, message):
        fields = message.split(",")
        ts = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
        os.rename("{}.csv".format(fields[0]), "{}_{}.csv".format(fields[0],ts))
        try:
            file = open("{}.csv".format(fields[0]), "x")
            file.close()
        except FileExistsError:
            print("File was not restarted correctly")
                        
    

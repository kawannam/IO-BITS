import pickle
import paho.mqtt.publish as publish


class EventHandler:
    EVENT_NUM = 100
    DATA_FILE = "press_data.p"
    NAMES = ["A", "B", "C", "D"]
    BUTTONS = ["A", "B"]
    RESPONSE = "iobits/DataResponse"
    RESPONSE_POINTS = "iobits/DataResponsePoints"

    def __init__(self):
        try:
            file = open(EventHandler.DATA_FILE, "x")
            file.close()
            init_data = {}
            for name in EventHandler.NAMES:
                init_data[name] = {}
                for button in EventHandler.BUTTONS:
                    init_data[name][button] = 0
            with open(EventHandler.DATA_FILE, "wb") as data_file:
                pickle.dump(init_data, data_file)
        except FileExistsError:
            pass

        for name in EventHandler.NAMES:
            try:
                file = open("{}.csv".format(name), "x")
                file.close()
            except FileExistsError:
                continue

        with open(EventHandler.DATA_FILE, "rb") as file:
            self.press_data = pickle.load(file)

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

        self.press_data[fields[0]][fields[1]] += 1
        with open(EventHandler.DATA_FILE, "wb") as file:
            pickle.dump(self.press_data, file)

    def data_request_event(self, message, server):
        fields = message.split(",")
        with open("{}.csv".format(fields[0]), "r") as file:
            pts = EventHandler.last_n_points(fields[0], file, EventHandler.EVENT_NUM)
            counts = ",".join([str(self.press_data[fields[0]][b]).zfill(4) for b in EventHandler.BUTTONS])
            publish.single(EventHandler.RESPONSE, payload=",".join([fields[0], counts,
                                                                    str(len(pts)).zfill(3)]).rstrip(","),
                           hostname=server)
            if len(pts) > 0:
                publish.multiple(pts, hostname=server)
            print("Done responding to data request.")

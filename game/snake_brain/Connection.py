import dbus

class Connection:

    def __init__(self):
        bus = dbus.SessionBus()
        self.dbus_obj = bus.get_object('org.torchlight.Inputs', '/inputs')

    def getDbInputs(self):
        inputs_size = self.dbus_obj.get_dbus_method('getInputsSize', dbus_interface='local.controller.Controller')()
        inputs = []
        buffer = []

        for index in range(inputs_size):
            input = self.dbus_obj.get_dbus_method('getAtInputs', dbus_interface='local.controller.Controller')(index)
            buffer.append(input.conjugate())

            if index % 8 == 0:
                inputs.append(buffer.copy())
                buffer.clear()
        return inputs

    def getDbTarget(self):
        target_size = self.dbus_obj.get_dbus_method('getTargetSize', dbus_interface='local.controller.Controller')()
        target = []
        buffer = []
        for index in range(target_size):
            item_target = self.dbus_obj.get_dbus_method('getAtTarget', dbus_interface='local.controller.Controller')(index)
            buffer.append(item_target.conjugate())

            if index % 3 == 0:
                target.append(buffer.copy())
                buffer.clear()
        return target

    def getInput(self):
        #DERECHA, IZQUIERDA, ABAJO, ARRIBA + fruta
        input = []

        for index in range(8):
            buffer = self.dbus_obj.get_dbus_method('getAtDistance', dbus_interface='local.controller.Controller')(index)
            input.append(buffer.conjugate())

        return input

    def sendMove(self, ann_output):
        threshold = self.dbus_obj.get_dbus_method('getThreshold', dbus_interface='local.controller.Controller')()

        if ann_output.item(0) > ann_output.item(1) and ann_output.item(0) > threshold:
            direction = 1
        elif ann_output.item(1) > ann_output.item(0) and ann_output.item(1) > threshold:
            direction = 2
        else:
            direction = 3

        '''
                # Izquierda
                if ann_output.item(0) > threshold:
                    direction = 1
                # Derecha
                elif ann_output.item(1) > threshold:
                    direction = 2
                # Frente
                elif ann_output.item(2) > threshold:
                    direction = 3
                '''
        #print ("direction", direction)
        self.dbus_obj.get_dbus_method('newDirection', dbus_interface='local.controller.Controller')(direction)

    def sendVerbose(self, mean_error, epoch):
        self.dbus_obj.get_dbus_method('setLabelError', dbus_interface='local.controller.Controller')(mean_error, epoch)

    def isPredict(self):
        return self.dbus_obj.get_dbus_method('isPredict', dbus_interface='local.controller.Controller')()

    def getEpochs(self):
        return self.dbus_obj.get_dbus_method('getEpochs', dbus_interface='local.controller.Controller')()

    def run(self):
        self.dbus_obj.get_dbus_method('runGame', dbus_interface='local.controller.Controller')()
import torch
import Brain
import signal
import Connection
import os
import time

if __name__ == '__main__':

    # Variables
    # -----------------------------------------------------------------
    brain = Brain.Brain()
    dbus = Connection.Connection()
    # -----------------------------------------------------------------

    # Cargar modelo entrenado
    # -----------------------------------------------------------------
    if dbus.isPredict():
        saved_brain = torch.load("../snake_brain/model-pro.pt")
        brain.model = saved_brain.model.eval()
    # -----------------------------------------------------------------

    # Crear datasets
    # -----------------------------------------------------------------
    else:
        torch.save(dbus.getDbInputs(), "../snake_brain/inputs_db.pt")
        torch.save(dbus.getDbTarget(), "../snake_brain/target_db.pt")
    # -----------------------------------------------------------------

    # Fase de entrenamiento
    # -----------------------------------------------------------------
    if not dbus.isPredict():
        brain.train(torch.load('../snake_brain/inputs_db.pt'),
                       torch.load('../snake_brain/target_db.pt'), dbus.getEpochs(), dbus)
        dbus.run()
    # -----------------------------------------------------------------

    # Guardar modelo entrenado
    # -----------------------------------------------------------------
    if not dbus.isPredict():
        torch.save(brain, "../snake_brain/model.pt")
    # -----------------------------------------------------------------

    # Fase de predicciones
    # -----------------------------------------------------------------
    while os.getppid() != 1:
        time.sleep(.42)
        dbus.sendMove(brain.predict(dbus.getInput()))
    # -----------------------------------------------------------------

    # Liberar memoria dbus
    # -----------------------------------------------------------------
    del dbus.dbus_obj
    # -----------------------------------------------------------------

import torch
import numpy as np
import Connection

class Brain:
    # Definir numero de neuronas en cada capa
    inputs_layer, hidden_layer1, hidden_layer2, output_layer = 8, 4, 4, 3
    mean_error = []

    def __init__(self):
        # Definir arquitectura del modelo neuronal
        self.model = torch.nn.Sequential(
            torch.nn.Linear(self.inputs_layer, self.hidden_layer1, bias=True),
            torch.nn.ReLU(),
            torch.nn.Linear(self.hidden_layer1, self.output_layer, bias=True),
#            torch.nn.ReLU(),
#            torch.nn.Linear(self.hidden_layer2, self.output_layer, bias=True),
        )

        # Definir funcion de perdida (Error Cuadratico Medio)
        self.loss_function = torch.nn.MSELoss()

        # Definir algoritmo de optimizacion (Descenso de Gradiente Estocastico)
        self.optimizer = torch.optim.SGD(self.model.parameters(), lr=0.1)

    def train(self, inputs, target, epochs, dbus):
        # Convertir datasets (listas) a tensores
        print(inputs)
        print(target)
        print(len(inputs), len(target))

        inputs.pop(0)
        target.pop(0)

        print(inputs)
        print(target)
        print(len(inputs), len(target))

        inputs = torch.from_numpy(np.array(inputs, dtype="int16")).float()
        target = torch.from_numpy(np.array(target, dtype="int16")).float()


        for epoch in range(epochs):
            # Aplicar función de propagación
            output = self.model(inputs)

            # Calculo del error
            error = self.loss_function(output, target)

            # Almacenar valor de error para el promedio
            self.mean_error.append(error.item())

            # Poner los gradientes a cero
            self.optimizer.zero_grad()

            # Calcular gradientes mediante Backpropagation (las derivadas finales de los pesos)
            error.backward()

            # Actualizar los pesos mediante el algoritmo de optimizacion
            self.optimizer.step()

            # Visualizar media del error
            print(np.mean(self.mean_error), " ", epoch)

            # Enviar media del error a snake
            dbus.sendVerbose(np.mean(self.mean_error), epoch)


    def predict(self, inputs):
        print("outputs", self.model(torch.from_numpy(np.array(inputs)).float()).sigmoid())
        return self.model(torch.from_numpy(np.array(inputs)).float()).sigmoid().detach().numpy()
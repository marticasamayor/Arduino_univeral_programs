# versió vella



import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

class ArduinoSerialPlotter:
    def __init__(self, port='/dev/ttyUSB0', baudrate=9600, max_samples=1000, 
                 initialization_samples=10, window_title='Arduino Real-Time Plotter'):
        """
        Inicializa el ploteador de datos en tiempo real desde Arduino
        
        Parámetros:
        - port: Puerto serie del Arduino
        - baudrate: Velocidad de comunicación
        - max_samples: Número máximo de muestras a mostrar
        - initialization_samples: Número de muestras para inicialización
        - window_title: Título de la ventana de gráficos
        """
        self.ser = serial.Serial(port, baudrate)
        self.max_samples = max_samples
        self.initialization_samples = initialization_samples
        self.window_title = window_title
        
        # Configuración de la figura y subplots
        self.fig, self.ax = plt.subplots(figsize=(12, 6))
        self.fig.suptitle(window_title)
        self.lines = []
        
        # Datos para el gráfico
        self.x_data = np.arange(max_samples)
        self.y_data = []
        
        # Banderas y contadores de inicialización
        self.initialization_data = []
        self.initialization_complete = False
        self.num_variables = None
    
    def detect_variables(self):
        """
        Detecta el número de variables durante la fase de inicialización
        """
        while len(self.initialization_data) < self.initialization_samples:
            try:
                line = self.ser.readline().decode('utf-8').strip()
                values = [float(val) for val in line.split()]
                
                # Almacena datos de inicialización
                self.initialization_data.append(values)
                
                # Actualiza número de variables si cambia
                if self.num_variables is None or self.num_variables < len(values):
                    self.num_variables = len(values)
                else:
                    print(f"Advertencia: Número de variables inconsistente. Esperado: {self.num_variables}, Recibido: {len(values)}")
            
            except Exception as e:
                print(f"Error durante la inicialización: {e}")
        
        self.initialization_complete = True
        print(f"Inicialización completa. Número de variables detectadas: {self.num_variables}")
    
    def update_plot(self, frame):
        """
        Actualiza el gráfico en cada frame
        """
        # Fase de inicialización
        if not self.initialization_complete:
            self.detect_variables()
            
            # Prepara líneas después de la inicialización
            for _ in range(self.num_variables):
                line, = self.ax.plot([], [], linewidth=2)
                self.lines.append(line)
            
            self.ax.set_xlim(0, self.max_samples)
            self.ax.set_xlabel('Muestras')
            self.ax.set_ylabel('Valores')
            self.ax.grid(True)
            
            return self.lines
        
        try:
            # Lee línea del puerto serie
            line = self.ser.readline().decode('utf-8').strip()
            values = [float(val) for val in line.split()]
            
            # Añade nuevos datos
            self.y_data.append(values)
            if len(self.y_data) > self.max_samples:
                self.y_data.pop(0)
            
            # Actualiza cada línea con sus datos
            for i, line in enumerate(self.lines):
                column_data = [row[i] for row in self.y_data]
                line.set_data(self.x_data[:len(column_data)], column_data)
            
            # Ajusta límites dinámicamente
            self.ax.relim()
            self.ax.autoscale_view()
            
            return self.lines
        
        except Exception as e:
            print(f"Error al procesar datos: {e}")
            return self.lines
    
    def start(self):
        """
        Inicia la animación y el gráfico en tiempo real
        """
        ani = animation.FuncAnimation(
            self.fig, 
            self.update_plot, 
            interval=50,  # Actualización cada 50ms 
            blit=True
        )
        plt.tight_layout()
        plt.show()

# Ejemplo de uso
if __name__ == '__main__':
    plotter = ArduinoSerialPlotter(
        port='COM14',  # Ajusta según tu puerto
        baudrate=115200,
        max_samples=100,
        initialization_samples=10,
        window_title='Graphic from Arduino'
    )
    plotter.start()
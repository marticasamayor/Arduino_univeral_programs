import sys
import csv
from datetime import datetime
import serial.tools.list_ports
import serial
import numpy as np
import matplotlib
matplotlib.use('QtAgg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qtagg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

from PySide6.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, 
    QWidget, QComboBox, QLabel, QPushButton, QSpinBox, QTextEdit,
    QCheckBox, QFileDialog, QMessageBox, QGroupBox
)
from PySide6.QtCore import QTimer, Signal, QObject, QThread, Qt

class SerialWorker(QObject):
    """Worker para lectura de datos serie en un hilo separado"""
    data_received = Signal(list)
    error_occurred = Signal(str)
    connection_status = Signal(bool, str)

    def __init__(self):
        super().__init__()
        self.ser = None
        self.running = False
        self._port = None
        self._baudrate = None

    def connect_serial(self, port, baudrate):
        """Conecta al puerto serie"""
        try:
            if self.ser and self.ser.is_open:
                self.ser.close()
            
            self.ser = serial.Serial(port, baudrate, timeout=0.1)
            self._port = port
            self._baudrate = baudrate
            self.running = True
            self.connection_status.emit(True, f"Conectado a {port}")
        except Exception as e:
            self.connection_status.emit(False, f"Error: {e}")
            self.error_occurred.emit(f"Error al abrir puerto: {e}")

    def read_data(self):
        """Lee datos del puerto serie"""
        if not self.running or not self.ser or not self.ser.is_open:
            return

        try:
            if self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    # Intenta parsear los valores
                    values = []
                    for val in line.split():
                        try:
                            values.append(float(val))
                        except ValueError:
                            continue
                    
                    if values:
                        self.data_received.emit(values)
        except Exception as e:
            self.error_occurred.emit(f"Error leyendo datos: {e}")

    def disconnect_serial(self):
        """Desconecta del puerto serie"""
        self.running = False
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.connection_status.emit(False, "Desconectado")

class PercentageSpinBox(QSpinBox):
    """Custom QSpinBox que incrementa/decrementa por 10%"""
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setMinimum(10)
        self.setMaximum(10000)
        self.setValue(100)

    def stepBy(self, steps):
        """Override step method para incrementar/decrementar por 10%"""
        current_value = self.value()
        step_size = max(1, int(current_value * 0.1))
        new_value = current_value + steps * step_size
        new_value = max(self.minimum(), min(new_value, self.maximum()))
        self.setValue(new_value)

class ArduinoSerialPlotterGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Arduino Serial Plotter Pro")
        self.setGeometry(100, 100, 1400, 700)

        # Variables de estado
        self.is_connected = False
        self.is_paused = False
        self.lines = []
        self.axes = []  # Lista de ejes para múltiples gráficas
        self.y_data = []
        self.x_data = None
        self.num_variables = 0
        self.colors = plt.cm.tab10.colors
        self.sample_counter = 0  # Contador global de muestras
        self.separate_plots = False  # Modo de gráficas separadas
        
        # Setup UI
        self.setup_ui()
        
        # Setup worker thread
        self.worker_thread = QThread()
        self.serial_worker = SerialWorker()
        self.serial_worker.moveToThread(self.worker_thread)
        
        # Conexiones de señales
        self.serial_worker.data_received.connect(self.handle_new_data)
        self.serial_worker.error_occurred.connect(self.handle_error)
        self.serial_worker.connection_status.connect(self.update_status)
        
        # Timer para lectura (siempre activo cuando conectado)
        self.data_timer = QTimer()
        self.data_timer.timeout.connect(self.serial_worker.read_data)
        self.data_timer.setInterval(20)  # 20ms para mejor rendimiento
        
        # Inicia el thread
        self.worker_thread.start()

    def setup_ui(self):
        """Configura la interfaz de usuario"""
        main_layout = QHBoxLayout()
        
        # Layout izquierdo (controles y gráfica)
        left_layout = QVBoxLayout()
        
        # Grupo de conexión
        connection_group = QGroupBox("Connexió")
        connection_layout = QHBoxLayout()
        
        self.port_combo = QComboBox()
        self.refresh_ports()
        connection_layout.addWidget(QLabel("Port:"))
        connection_layout.addWidget(self.port_combo)
        
        refresh_btn = QPushButton("🔄")
        refresh_btn.setMaximumWidth(40)
        refresh_btn.clicked.connect(self.refresh_ports)
        connection_layout.addWidget(refresh_btn)
        
        self.baudrate_combo = QComboBox()
        baudrates = [9600, 19200, 38400, 57600, 115200, 230400, 460800]
        self.baudrate_combo.addItems(map(str, baudrates))
        self.baudrate_combo.setCurrentText('115200')
        connection_layout.addWidget(QLabel("Baudrate:"))
        connection_layout.addWidget(self.baudrate_combo)
        
        self.connect_btn = QPushButton("Connectar")
        self.connect_btn.clicked.connect(self.toggle_connection)
        connection_layout.addWidget(self.connect_btn)
        
        connection_group.setLayout(connection_layout)
        left_layout.addWidget(connection_group)
        
        # Grupo de controles de visualización
        control_group = QGroupBox("Controls de Visualització")
        control_layout = QHBoxLayout()
        
        self.max_samples_spin = PercentageSpinBox()
        control_layout.addWidget(QLabel("Samples:"))
        control_layout.addWidget(self.max_samples_spin)
        
        self.pause_btn = QPushButton("Pausa")
        self.pause_btn.clicked.connect(self.toggle_pause)
        self.pause_btn.setEnabled(False)
        control_layout.addWidget(self.pause_btn)
        
        self.clear_btn = QPushButton("Netejar")
        self.clear_btn.clicked.connect(self.clear_plot)
        control_layout.addWidget(self.clear_btn)
        
        self.autoscroll_check = QCheckBox("Auto-scroll")
        self.autoscroll_check.setChecked(True)
        control_layout.addWidget(self.autoscroll_check)
        
        self.separate_plots_check = QCheckBox("Gràfiques separades")
        self.separate_plots_check.setChecked(False)
        self.separate_plots_check.stateChanged.connect(self.toggle_plot_mode)
        control_layout.addWidget(self.separate_plots_check)
        
        self.export_btn = QPushButton("Exportar CSV")
        self.export_btn.clicked.connect(self.export_data)
        control_layout.addWidget(self.export_btn)
        
        control_group.setLayout(control_layout)
        left_layout.addWidget(control_group)
        
        # Gráfica con toolbar de navegación
        self.figure = Figure(figsize=(12, 6))
        self.canvas = FigureCanvas(self.figure)
        self.ax = self.figure.add_subplot(111)
        self.axes = [self.ax]  # Inicialmente un solo eje
        self.ax.set_xlabel('Mostres')
        self.ax.set_ylabel('Valors')
        self.ax.grid(True, alpha=0.3)
        
        # Añadir toolbar de navegación para zoom y pan
        self.toolbar = NavigationToolbar(self.canvas, self)
        left_layout.addWidget(self.toolbar)
        left_layout.addWidget(self.canvas)
        
        # Estado
        self.status_label = QLabel("Desconnectat")
        self.status_label.setStyleSheet("padding: 5px; background-color: #ffcccc;")
        left_layout.addWidget(self.status_label)
        
        # Panel derecho (estadísticas)
        right_layout = QVBoxLayout()
        
        stats_group = QGroupBox("Estadístiques")
        stats_layout = QVBoxLayout()
        
        self.stats_text = QTextEdit()
        self.stats_text.setReadOnly(True)
        self.stats_text.setMinimumWidth(280)
        stats_layout.addWidget(self.stats_text)
        
        stats_group.setLayout(stats_layout)
        right_layout.addWidget(stats_group)
        
        # Info
        info_group = QGroupBox("Informació")
        info_layout = QVBoxLayout()
        
        self.info_text = QTextEdit()
        self.info_text.setReadOnly(True)
        self.info_text.setMaximumHeight(150)
        self.info_text.setText(
            "📊 Arduino Serial Plotter Pro\n\n"
            "• Connecta el teu Arduino\n"
            "• Selecciona port i baudrate\n"
            "• Envia valors separats per espais\n"
            "• Exemple: '1.5 2.3 4.1\\n'"
        )
        info_layout.addWidget(self.info_text)
        
        info_group.setLayout(info_layout)
        right_layout.addWidget(info_group)
        
        # Log de datos
        log_group = QGroupBox("Log de Dades Rebudes")
        log_layout = QVBoxLayout()
        
        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setMaximumHeight(200)
        log_layout.addWidget(self.log_text)
        
        clear_log_btn = QPushButton("Netejar Log")
        clear_log_btn.clicked.connect(lambda: self.log_text.clear())
        log_layout.addWidget(clear_log_btn)
        
        log_group.setLayout(log_layout)
        right_layout.addWidget(log_group)
        
        right_layout.addStretch()
        
        # Layout principal
        main_layout.addLayout(left_layout, 3)
        main_layout.addLayout(right_layout, 1)
        
        container = QWidget()
        container.setLayout(main_layout)
        self.setCentralWidget(container)

    def refresh_ports(self):
        """Actualiza la lista de puertos disponibles"""
        current = self.port_combo.currentText()
        self.port_combo.clear()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo.addItems(ports)
        
        if current in ports:
            self.port_combo.setCurrentText(current)

    def toggle_connection(self):
        """Conecta o desconecta del puerto serie"""
        if not self.is_connected:
            port = self.port_combo.currentText()
            baudrate = int(self.baudrate_combo.currentText())
            
            if not port:
                QMessageBox.warning(self, "Error", "Selecciona un port sèrie")
                return
            
            self.serial_worker.connect_serial(port, baudrate)
            self.data_timer.start()
        else:
            self.data_timer.stop()
            self.serial_worker.disconnect_serial()

    def toggle_pause(self):
        """Pausa o reanuda la visualización (no la lectura)"""
        self.is_paused = not self.is_paused
        if self.is_paused:
            self.pause_btn.setText("Reprendre")
        else:
            self.pause_btn.setText("Pausa")

    def clear_plot(self):
        """Limpia los datos de la gráfica"""
        self.y_data = []
        self.sample_counter = 0
        self.figure.clear()
        
        # Recrear ejes según el modo actual
        if self.separate_plots and self.num_variables > 0:
            self.axes = []
            for i in range(self.num_variables):
                ax = self.figure.add_subplot(self.num_variables, 1, i + 1)
                ax.set_ylabel(f'Variable {i+1}')
                ax.grid(True, alpha=0.3)
                self.axes.append(ax)
            self.axes[-1].set_xlabel('Mostres')
        else:
            self.ax = self.figure.add_subplot(111)
            self.axes = [self.ax]
            self.ax.set_xlabel('Mostres')
            self.ax.set_ylabel('Valors')
            self.ax.grid(True, alpha=0.3)
        
        self.lines = []
        self.canvas.draw()
        self.stats_text.clear()

    def toggle_plot_mode(self):
        """Cambia entre modo gráfica única y gráficas separadas"""
        self.separate_plots = self.separate_plots_check.isChecked()
        
        if not self.y_data:
            return
        
        # Limpiar figura
        self.figure.clear()
        self.lines = []
        
        if self.separate_plots:
            # Crear múltiples subplots
            self.axes = []
            for i in range(self.num_variables):
                ax = self.figure.add_subplot(self.num_variables, 1, i + 1)
                color = self.colors[i % len(self.colors)]
                line, = ax.plot([], [], linewidth=2, color=color)
                self.lines.append(line)
                
                ax.set_ylabel(f'Variable {i+1}', color=color)
                ax.tick_params(axis='y', labelcolor=color)
                ax.grid(True, alpha=0.3)
                self.axes.append(ax)
            
            # Solo el último tiene etiqueta X
            self.axes[-1].set_xlabel('Mostres')
            
            # Sincronizar ejes X de todos los subplots
            self.sync_axes()
        else:
            # Crear un solo plot
            self.ax = self.figure.add_subplot(111)
            self.axes = [self.ax]
            
            for i in range(self.num_variables):
                color = self.colors[i % len(self.colors)]
                line, = self.ax.plot([], [], linewidth=2, 
                                   label=f'Variable {i+1}', 
                                   color=color)
                self.lines.append(line)
            
            self.ax.set_xlabel('Mostres')
            self.ax.set_ylabel('Valors')
            self.ax.grid(True, alpha=0.3)
            self.ax.legend(loc='upper right')
        
        # Ajustar espaciado
        self.figure.tight_layout()
        
        # Redibujar con los datos existentes
        if self.y_data and not self.is_paused:
            self.update_plot(self.y_data[-1])

    def sync_axes(self):
        """Sincroniza los ejes X de todos los subplots para movimiento conjunto"""
        if len(self.axes) > 1:
            # Compartir el eje X entre todos los subplots
            for i in range(1, len(self.axes)):
                self.axes[i].sharex(self.axes[0])

    def handle_new_data(self, values):
        """Maneja los nuevos datos recibidos (siempre se guardan)"""
        # Añadir al log
        timestamp = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        values_str = ' '.join([f'{v:.3f}' for v in values])
        log_entry = f"[{timestamp}] {values_str}"
        self.log_text.append(log_entry)
        
        # Auto-scroll del log
        scrollbar = self.log_text.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())
        
        # Inicializa líneas si es necesario
        if not self.lines and values:
            self.num_variables = len(values)
            
            if self.separate_plots:
                # Crear múltiples subplots
                self.figure.clear()
                self.axes = []
                for i in range(self.num_variables):
                    ax = self.figure.add_subplot(self.num_variables, 1, i + 1)
                    color = self.colors[i % len(self.colors)]
                    line, = ax.plot([], [], linewidth=2, color=color)
                    self.lines.append(line)
                    
                    ax.set_ylabel(f'Variable {i+1}', color=color)
                    ax.tick_params(axis='y', labelcolor=color)
                    ax.grid(True, alpha=0.3)
                    self.axes.append(ax)
                
                self.axes[-1].set_xlabel('Mostres')
                
                # Sincronizar ejes X
                self.sync_axes()
            else:
                # Crear un solo plot
                for i in range(self.num_variables):
                    color = self.colors[i % len(self.colors)]
                    line, = self.ax.plot([], [], linewidth=2, 
                                       label=f'Variable {i+1}', 
                                       color=color)
                    self.lines.append(line)
                
                self.ax.legend(loc='upper right')
            
            self.figure.tight_layout()
        
        # Verifica consistencia de variables
        if len(values) != self.num_variables:
            return
        
        # Siempre guardamos los datos (incluso en pausa)
        self.y_data.append(values)
        self.sample_counter += 1
        
        # Solo actualizamos la gráfica si no está en pausa
        if not self.is_paused:
            self.update_plot(values)

    def update_plot(self, current_values):
        """Actualiza la gráfica con los datos guardados"""
        if not self.y_data:
            return
        
        max_samples = self.max_samples_spin.value()
        
        # Determinar qué datos mostrar
        if self.autoscroll_check.isChecked():
            # Mostrar solo las últimas 'max_samples' muestras
            display_data = self.y_data[-max_samples:]
            x_start = max(0, self.sample_counter - max_samples)
            x_vals = np.arange(x_start, x_start + len(display_data))
        else:
            # Mostrar todos los datos (permitir navegación manual)
            display_data = self.y_data
            x_vals = np.arange(len(display_data))
        
        # Actualiza líneas según el modo
        if self.separate_plots:
            # Actualizar cada subplot individualmente
            for i, (line, ax) in enumerate(zip(self.lines, self.axes)):
                column_data = [row[i] for row in display_data]
                line.set_data(x_vals, column_data)
                
                # Ajusta ejes solo si auto-scroll está activo
                if self.autoscroll_check.isChecked():
                    ax.relim()
                    ax.autoscale_view()
        else:
            # Actualizar una sola gráfica
            for i, line in enumerate(self.lines):
                column_data = [row[i] for row in display_data]
                line.set_data(x_vals, column_data)
            
            # Ajusta ejes solo si auto-scroll está activo
            if self.autoscroll_check.isChecked():
                self.ax.relim()
                self.ax.autoscale_view()
        
        # Redibuja
        self.canvas.draw_idle()
        
        # Actualiza estadísticas
        self.update_statistics(current_values)

    def update_statistics(self, current_values):
        """Actualiza el texto de estadísticas"""
        if not self.y_data:
            return
        
        stats_text = f"📈 Mostres totals: {len(self.y_data)}\n\n"
        
        for i in range(self.num_variables):
            column_data = [row[i] for row in self.y_data]
            
            if column_data:
                min_val = min(column_data)
                max_val = max(column_data)
                mean_val = np.mean(column_data)
                std_val = np.std(column_data)
                current_val = current_values[i]
                
                color_hex = matplotlib.colors.rgb2hex(self.colors[i % len(self.colors)])
                
                stats_text += f"<b style='color:{color_hex}'>Variable {i+1}:</b><br>"
                stats_text += f"  Actual: <b>{current_val:.3f}</b><br>"
                stats_text += f"  Min: {min_val:.3f}<br>"
                stats_text += f"  Max: {max_val:.3f}<br>"
                stats_text += f"  Mitjana: {mean_val:.3f}<br>"
                stats_text += f"  Desv. Est: {std_val:.3f}<br><br>"
        
        self.stats_text.setHtml(stats_text)

    def update_status(self, connected, message):
        """Actualiza el estado de la conexión"""
        self.is_connected = connected
        self.status_label.setText(message)
        
        if connected:
            self.status_label.setStyleSheet("padding: 5px; background-color: #ccffcc;")
            self.connect_btn.setText("Desconnectar")
            self.pause_btn.setEnabled(True)
            self.port_combo.setEnabled(False)
            self.baudrate_combo.setEnabled(False)
        else:
            self.status_label.setStyleSheet("padding: 5px; background-color: #ffcccc;")
            self.connect_btn.setText("Connectar")
            self.pause_btn.setEnabled(False)
            self.pause_btn.setText("Pausa")
            self.is_paused = False
            self.port_combo.setEnabled(True)
            self.baudrate_combo.setEnabled(True)

    def export_data(self):
        """Exporta los datos a un archivo CSV"""
        if not self.y_data:
            QMessageBox.warning(self, "Sense dades", "No hi ha dades per exportar")
            return
        
        filename, _ = QFileDialog.getSaveFileName(
            self, "Desar dades", 
            f"dades_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv",
            "CSV Files (*.csv)"
        )
        
        if filename:
            try:
                with open(filename, 'w', newline='') as f:
                    writer = csv.writer(f)
                    headers = [f'Variable_{i+1}' for i in range(self.num_variables)]
                    writer.writerow(headers)
                    writer.writerows(self.y_data)
                
                QMessageBox.information(self, "Èxit", f"Dades desades a:\n{filename}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Error al desar: {e}")

    def handle_error(self, error_msg):
        """Maneja errores de conexión o lectura"""
        print(f"Error: {error_msg}")

    def closeEvent(self, event):
        """Limpieza al cerrar la aplicación"""
        self.data_timer.stop()
        self.serial_worker.disconnect_serial()
        self.worker_thread.quit()
        self.worker_thread.wait()
        event.accept()

def main():
    app = QApplication(sys.argv)
    app.setStyle('Fusion')
    plotter = ArduinoSerialPlotterGUI()
    plotter.show()
    sys.exit(app.exec())

if __name__ == '__main__':
    main()
import sys
import math
import serial
from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6.QtCore import QTimer
import pyqtgraph as pg

# =========================================================
# SERIAL SETTINGS
# =========================================================

SERIAL_PORT = "COM9"      # CHANGE THIS
BAUD_RATE = 115200

# =========================================================
# EXPECTED SERIAL FORMAT
# =========================================================
# x,y,heading,stepCount,moving
#
# Example:
# 10.5,20.3,45.0,120,1
#
# moving:
# 1 = moving
# 0 = stopped
# =========================================================


class TrackerWindow(QMainWindow):

    def __init__(self):
        super().__init__()

        # -------------------------------------------------
        # Window Setup
        # -------------------------------------------------

        self.setWindowTitle("Serial XY Motion Tracker")
        self.resize(1000, 800)

        # -------------------------------------------------
        # Plot Widget
        # -------------------------------------------------

        self.plot_widget = pg.PlotWidget()

        self.setCentralWidget(self.plot_widget)

        self.plot_widget.setBackground("w")

        self.plot_widget.showGrid(x=True, y=True)

        self.plot_widget.setAspectLocked(True)

        self.plot_widget.setLabel("left", "Y Position")

        self.plot_widget.setLabel("bottom", "X Position")

        self.plot_widget.setXRange(-50, 50)

        self.plot_widget.setYRange(-50, 50)

        # -------------------------------------------------
        # Data Arrays
        # -------------------------------------------------

        self.x_data = []

        self.y_data = []

        # -------------------------------------------------
        # Trail Plot
        # -------------------------------------------------

        self.trail_plot = self.plot_widget.plot(
            [],
            [],
            pen=pg.mkPen(color='b', width=2)
        )

        # -------------------------------------------------
        # Current Position Marker
        # -------------------------------------------------

        self.position_marker = pg.ScatterPlotItem(
            size=12,
            brush='r'
        )

        self.plot_widget.addItem(self.position_marker)

        # -------------------------------------------------
        # Direction Arrow
        # -------------------------------------------------

        self.arrow = pg.ArrowItem(
            angle=0,
            headLen=20,
            tipAngle=30,
            baseAngle=20,
            brush='g'
        )

        self.plot_widget.addItem(self.arrow)

        # -------------------------------------------------
        # Open Serial Port
        # -------------------------------------------------

        try:

            self.serial_port = serial.Serial(
                SERIAL_PORT,
                BAUD_RATE,
                timeout=0.1
            )

            print(f"Connected to {SERIAL_PORT}")

        except Exception as e:

            print("Serial connection failed")

            print(e)

            sys.exit()

        # -------------------------------------------------
        # Timer
        # -------------------------------------------------

        self.timer = QTimer()

        self.timer.timeout.connect(self.read_serial_data)

        self.timer.start(20)

    # =====================================================
    # SERIAL READING FUNCTION
    # =====================================================

    def read_serial_data(self):

        try:

            # Check if data available
            if self.serial_port.in_waiting:

                # Read line
                raw_data = self.serial_port.readline().decode().strip()

                # Ignore empty lines
                if raw_data == "":
                    return

                # Debug print
                print(raw_data)

                # Split CSV
                values = raw_data.split(',')

                # Ensure correct number of values
                if len(values) != 5:
                    return

                # -------------------------------------------------
                # Parse Data
                # -------------------------------------------------

                x = float(values[0])

                y = float(values[1])

                heading = float(values[2])

                step_count = int(values[3])

                moving = bool(int(values[4]))

                # -------------------------------------------------
                # Store Path
                # -------------------------------------------------

                self.x_data.append(x)

                self.y_data.append(y)

                # Limit trail size
                if len(self.x_data) > 500:

                    self.x_data.pop(0)

                    self.y_data.pop(0)

                # -------------------------------------------------
                # Update Trail
                # -------------------------------------------------

                self.trail_plot.setData(
                    self.x_data,
                    self.y_data
                )

                # -------------------------------------------------
                # Update Current Position
                # -------------------------------------------------

                self.position_marker.setData(
                    [x],
                    [y]
                )

                # -------------------------------------------------
                # Update Arrow
                # -------------------------------------------------

                self.arrow.setPos(x, y)

                self.arrow.setStyle(
                    angle=-heading + 90
                )

                # -------------------------------------------------
                # Change Marker Color Based on Moving State
                # -------------------------------------------------

                if moving:

                    self.position_marker.setBrush('g')

                else:

                    self.position_marker.setBrush('r')

        except Exception as e:

            print("Data Error:", e)


# =========================================================
# MAIN
# =========================================================

app = QApplication(sys.argv)

window = TrackerWindow()

window.show()

sys.exit(app.exec())
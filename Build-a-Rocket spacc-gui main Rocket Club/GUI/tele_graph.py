import pyqtgraph                                 # Import the pyqtgraph library for real-time plotting
from pyqtgraph import PlotWidget                # Import the PlotWidget class for embedding plot widgets

class TelemetryGraph:                            # Define a helper class for managing telemetry plots

    def __init__(self, graph, legend=False):     # Constructor takes a PlotWidget instance and an optional legend flag
        self._graph = graph                      # Store the PlotWidget reference
        self._graph.setBackground('w')           # Set the plot background color to white

        self.styles = {'color': '#000000',       # Define default text styles for axis labels
                       'font-size': '12px'}

        self.y_limit = 30                        # Maximum number of data points to retain per line

        self._x = dict()                         # Dictionary mapping line names to their x-value lists
        self._y = dict()                         # Dictionary mapping line names to their y-value lists
        self._lines = dict()                     # Dictionary mapping line names to their PlotDataItem objects
        self._pen = dict()                       # Dictionary mapping line names to their pen (drawing style)

        if legend:                               # If a legend is requested
            self._graph.addLegend(offset=(0, 0)) # Add a legend to the graph at the top-left corner

    def addLine(self, name='default', color='black'):  # Create and register a new line with an optional color
        self._x[name] = [0]                      # Initialize x-data list for this line (starting at 0)
        self._y[name] = [0]                      # Initialize y-data list for this line (starting at 0)
        self._pen[name] = pyqtgraph.mkPen(color=color)  # Create a pen of the given color
        self._lines[name] = self._graph.plot(    # Plot the initial data and store the resulting PlotDataItem
            self._x[name], 
            self._y[name], 
            name=name
        )

    def plotData(self, x, y, name='default'):    # Append a new data point to the specified line and refresh it
        self._x[name].append(x)                  # Append the new x-value
        self._y[name].append(y)                  # Append the new y-value

        if len(self._x[name]) > self.y_limit:    # If beyond the retention limit,
            self._x[name] = self._x[name][1:]    # drop the oldest x-value
            self._y[name] = self._y[name][1:]    # drop the oldest y-value

        self._lines[name].setData(               # Update the PlotDataItem with trimmed data
            self._y[name], 
            self._x[name], 
            name=name, 
            pen=self._pen[name]
        )

    def setBackgroundColor(self, color='w'):     # Change the graph’s background color at runtime
        self._graph.setBackground(color)         # Apply the specified color

    def setTitle(self, name, color='black'):     # Set the graph’s title and its color
        self._graph.setTitle(                    # Apply title text, color, and font size
            name, 
            color=color, 
            size='12pt'
        )

    def setYLabel(self, name):                   # Label the vertical (left) axis
        self._graph.setLabel(                    # Use predefined text styles for consistency
            'left', 
            name, 
            **self.styles
        )

    def setXLabel(self, name):                   # Label the horizontal (bottom) axis
        self._graph.setLabel(
            'bottom', 
            name, 
            **self.styles
        )

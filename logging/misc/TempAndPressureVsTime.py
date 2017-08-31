Python 2.7.13 (v2.7.13:a06454b1afa1, Dec 17 2016, 20:42:59) [MSC v.1500 32 bit (Intel)] on win32
Type "copyright", "credits" or "license()" for more information.
>>> ##Linfengcollins 7/19/2017
import time
import matplotlib.pyplot as plt
import plotly.plotly as py
import plotly.graph_objs as go

temperatureFile=raw_input("Enter temperature Filename: ")
pressureFile=raw_input("Enter pressure Filename: ")



epochTemp = []
ambient = []
plate = []
sample = []
epochPres = []
pressure[]



def readTemp(x):
    with open(x) as f:
        for line in f:
            epochTemp.append(line.split()[0]) ##may not be able to use same epoch variable
            ambient.append(line.split()[1])
            plate.append(line.split()[2])
            sample.append(line.split()[3])
def readPressure(x):
    with open(x) as f:
        for line in f:
            epochPres.append(line.split()[0])
            pressure.append(line.split()[1])

            
readTemp(temperatureFile)
readPressure(pressureFile)

TemperatureSide = go.Scatter(
    x=epochTemp,
    y=ambient,
    y=plate,
    y=sample
    name='temperature data'
)
PressureSide = go.Scatter(
    x=epochPres,
    y=pressure,
    name='Pressure data',
    yaxis='y2'
)

data = [TemperatureSide, PressureSide]
layout = go.Layout(
    title='Temperature and Pressure vs. Time',
    yaxis=dict(
        title='Temperature'
    ),
    yaxis2=dict(
        title='Pressure',
        titlefont=dict(
            color='rgb(148, 103, 189)'
        ),
        tickfont=dict(
            color='rgb(148, 103, 189)'
        ),
        overlaying='y',
        side='right'
    )
)
fig = go.Figure(data=data, layout=layout)
plot_url = py.plot(fig, filename='TempAndPressureVsTime')


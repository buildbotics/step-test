#!/usr/bin/env python3

import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque

import matplotlib.pyplot as plt
import matplotlib.animation as animation


# plot class
class Plot:
  # constr
  def __init__(self, port, baud, max_len):
      self.sp = serial.Serial(port, baud)

      self.series = []
      for i in range(3):
          self.series.append(deque([0.0] * max_len))

      self.max_len = max_len
      self.incoming = ''


  def add(self, buf, value):
      if len(buf) < self.max_len:
          buf.append(value)
      else:
          buf.pop()
          buf.appendleft(value)


  def update(self, frame, axes):
      try:
          data = self.sp.read(self.sp.in_waiting)
          self.incoming += data.decode('utf-8')
      except Exception as e:
          print(e)
          return

      while True:
          i = self.incoming.find('\n')
          if i == -1: break
          line = self.incoming[0:i]
          self.incoming = self.incoming[i + 1:]

          try:
              print(line)
              data = [float(value) for value in line.split(',')]

              if len(data) == 3:
                  for i in range(3):
                      self.add(self.series[i], data[i])
                      axes[i].set_data(range(self.max_len), self.series[i])

          except ValueError: pass


  def close(self):
      self.sp.flush()
      self.sp.close()


description = "Plot velocity data in real-time"

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description = description)
  parser.add_argument('-p', '--port', default = '/dev/ttyUSB0')
  parser.add_argument('-b', '--baud', default = 115200, type = int)
  parser.add_argument('-m', '--max-width', default = 2000, type = int)
  args = parser.parse_args()

  plot = Plot(args.port, args.baud, args.max_width)

  # Set up animation
  fig = plt.figure()
  ax = plt.axes(xlim = (0, args.max_width), ylim = (-15000, 15000))
  axes = []

  for i in range(3):
      axis, = ax.plot([], [])
      axes.append(axis)

  anim = animation.FuncAnimation(fig, plot.update, fargs = [axes],
                                 interval = 100)

  plt.show()
  plot.close()

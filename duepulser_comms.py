#!/usr/bin/env python3
import serial.tools.list_ports as slp
import serial

class PulserCommunication():
  def __init__(self):
    self.connected = False

  def findSerial(self):
    self.port_list = list(slp.comports())
    # Find the arduino DUE
    self.port = None
    for port in self.port_list:
      try:
        if 'Arduino Due' in port.product:
          # This is the one
          self.due_port_obj = port
          self.port = port.device
      except TypeError:
        pass

  def connectSerial(self):
    try:
      self.arduino = serial.Serial(self.port, 9600, timeout=1)
      self.arduino.write(b'Hello\n')
      self.connected = True
      return self.arduino.readall()
    except serial.serialutil.SerialException:
      print('Incorrect serial port %s' % self.port)

  def pulserOn(self):
    self.arduino.write(b'On\n')
    return self.arduino.readall()

  def pulserOff(self):
    self.arduino.write(b'Off\n')
    return self.arduino.readall()

  def setFrequency(self, freq):
    ''' Frequency is an int, and should be limited from 1 to 1e6 '''
    freq = self.boundInt(freq, 1, 1000000)
    self.arduino.write( b'SetFrequency %i\n' % freq )
    return self.arduino.readall()

  def setCycles(self, cycles):
    ''' Cycles is an int, and should be limited from 1 to 1000 '''
    cycles = self.boundInt(cycles, 1, 1000) 
    self.arduino.write( b'SetCycles %i\n' % cycles)
    return self.arduino.readall()

  def boundInt(self, var, low, hi):
    var = int( var )
    var = min( var, hi )
    var = max( var, low )
    return var

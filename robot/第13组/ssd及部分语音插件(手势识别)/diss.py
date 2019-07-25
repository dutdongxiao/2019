
# -*- coding: utf-8 -*-
"""
Created on Tue Jul  2 22:18:42 2019

@author: liutianyi
"""
import serial
from robot.sdk.AbstractPlugin import AbstractPlugin
ser = serial.Serial("/dev/ttyS0", baudrate=115200)

class Plugin(AbstractPlugin):

    def handle(self, text, parsed):
        self.say('﻿好的', cache=True)
        ch = 'diss$' #input()
        ser.write(ch.encode('ascii'))

    def isValid(self, text, parsed):
        return "﻿摇头" in text


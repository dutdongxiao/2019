package com.wow.carcontroller;

public enum ControlNum {

    NNNN('0'),
    Forward('1'),
    Back('2'),
    Up('3'),
    Down('4'),
    Left('5'),
    Right('6'),
    Stop('7');

    private char com;
    ControlNum(char com) {
        this.com = com;
    }
    public char getCom() {
        return this.com;
    }
}

import RPi.GPIO as GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(22,GPIO.OUT,initial=GPIO.LOW)
while True:
    GPIO.output(22,GPIO.HIGH)
    GPIO.output(22,GPIO.LOW)
    

from threading import Thread, Lock, Condition
from time import sleep

LIMITE = 5

full = 0
empty = LIMITE

mutex = Condition()

# Produtor
def produce():
    global full
    global empty

    full += 1
    empty -= 1
    print("Produced {}".format(full))

def productor():
    global LIMITE
    global full
    global mutex

    while True:
        mutex.acquire()

        if full < LIMITE:
            produce()
        else:
            mutex.wait()
            
        if full == 1:
            mutex.notify_all()
            pass

        mutex.release()
        sleep(1)



# Consumidor
def consume():
    global full
    global empty

    full -= 1
    empty += 1
    print("Consumed {}".format(full))


def consumer():
    global empty
    global LIMITE
    global mutex

    while True:
        mutex.acquire()
        if empty == LIMITE:
            mutex.wait()
        
        if empty == 1:
            mutex.notify_all()

        consume()
        mutex.release()
        sleep(2)



produtor = Thread(target=productor, args=[])
produtor.start()

consumidor = Thread(target=consumer, args=[])
consumidor.start()

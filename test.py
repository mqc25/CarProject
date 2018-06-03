import pyServer
import json
import time
def main():
    pyServer.call()
    x=1
    y=1


    while 1:
        m_string = {
            "x": x,
            "y": y
        }
        data = json.dumps(m_string)

        pyServer.update(data)
        print (data)
        x +=1
        y+=1
        time.sleep(0.5)


main()

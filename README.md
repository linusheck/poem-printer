# The poem machine

## How it works - software

Inside the poem machine sits a ESP-8266, which is similar to an arduino, but it can connect to WiFi and is a bit more powerful. The ESP doesn't actually do much itself. Let's just run through what happens when you press the button on the poem machine:

1. You press the button on the poem machine.
2. The esp asks the server backend for the name of a poem. The server backend randomly selects a poem, and returns the name of it. For instance, that name would be 54fb64e9-0829-476e-92d2-38982c86f785.json.
3. The esp asks the server for image data from that particular poem, and it wants only the first n bytes. The poem is an image, not a text, because the standard printer font is kinda ugly. The printer can print bitmap images in a certain format, so you can just let it print an image with the poem written in a nicer font. So the server sends the printer an image of the rendered text, not the plain text, of the poem.
4. The server sees that it hasn't actually generated the poem image yet.
5. The server copies the poem text into a html file, that has some style attributes to make the poem look good.
6. The server makes a screenshot of the generated html file at the width of the printer (using wkhtmltoimage).
7. The server converts the resulting png into the bitmap format the printer uses.
8. The server stores the bitmap in buffer, with the poem name as key.
9. The server sends the first n bytes of the poem to the client in base64, along with information how long the poem is.
10. The client asks for more chunks of data, until it has hit the end of the poem. Every chunk of data is decoded from base64 into binary and directly transmitted to the printer in Serial.

That's it.

The code for the ESP is in this repo, and there is another repo for the server backend: <TODO: insert link>

## Hardware

### Electronics

This project uses this printer: https://www.adafruit.com/product/597 (from a cheaper & more unethical source) and a 19mm glowing button, as well as an ESP-8266 devboard.

### Enclosure

I made an enclosure in OpenSCAD. It should be somewhere in this repo. It's for 4mm wood.
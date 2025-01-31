<h1>Life Improvement Device</h1>
<h2><a href="https://www.youtube.com/embed/oOnCfNCNFV8?si=gfAQq7jsOvnHzt7z">Click here to see it in action!</a></h2>

<h2>Description</h2>
The life improvement device, currently, is an e-paper project. It is Soldered's Inkplate 6, which acts as a frontend for NWS radar images preprocessed by a raspberry pi. Most of the work thus far has involved finding tricks to overcome the limitations of e-ink displays. So I will get into that next.

<h2><a href="https://soldered.com/product/inkplate-6-6-e-paper-board">Inkplate 6</a></h2>
The Inkplate 6 is a high end driver for e-paper displays. E-paper is a unique platform, it uses charged pigments to form an image, rather than brightness. As a result, the display is unique, and easy on the eyes, but also difficult to navigate, mainly due to it's high refresh rate, and limitation in color (the Inkplate 6 is black and white, offering greyscale in between).

My goal in this project was to produce a home assistant, which is a common use case for e-paper hobby projects. What made my project different is I wanted the display to handle animation. Typically, image transitions on e-paper require what's called pixel inversion, the screen turns white, then black, then you see your image. This eliminates all lingering charges, and your image appears. However, this is *very* slow, full refreshes such as these can take over a second, even on high end displays like the Inkplate.

<h2>Partial Refresh</h2>
To avoid this full refresh, I used a feature offered by some e-paper displays, known as partial refresh. You see, when only part of the screen has changed, it is unnecessary to update the entire display, and it takes time to do so. Thus, if you only update a subset of the display, you get faster, more subtle updates.

<h2>Dithering</h2>
<p>I also previously alluded to the fact the Inkplate 6 supports only black and white. This includes a 3 bit greyscale. I opted to not use the greyscale, as a 1 bit monochrome bitmap would be more performant, and keep in mind, we're trying to smoothly imitate motion on an e-paper display. However, converting from 256/256/256 rgb in a .gif, straight to a bitmap was a non starter. Here is a frame of the NWS Buffalo weather radar, which I'll be using:</p>

<img src="https://github.com/user-attachments/assets/d13f6323-0eb5-4a88-8b55-f6e606de3f86">

<p>Here is that image converted to mono bmp, using a naiive threshold:</p>

<img src="https://github.com/user-attachments/assets/4275440c-57d0-4071-9e4f-315f1e7fca45">

<p>This would load fast, with the one drawback that it looks terrible.</p>

<p>Thankfully, the problem of giving the illusion of multiple colors, in an environment where there are few, is solved with a family of algorithms that do "dithering". Dithering describes the process of interweaving pixels, so that the pattern of weaved pixels gives the impression of a mix of the two colors. This process is commonly used in older computer games, as well as by comic books and cheap printers. I opted to use what's called a Bayer Matrix, which is used as a threshold map to produce ordered dithering. Essentially, rather than picking one threshold and applying it everywhere, you have a threshold matrix which is applied pixel by pixel. I chose to use a Bayer Matrix because the algorithm is fast, simple to implement, and *copes well with motion*. Algorithms such as Floyd Steinberg produce artifacts when movement takes place.</p>
<img src="https://github.com/user-attachments/assets/d3fe2687-e6f5-4f16-a290-31aa24acc182">

With the image reduced to a dithered monochrome bitmap, the Inkplate 6 can render the images, with a high refresh rate, but not an unbroken sense of continuity, as lengthy pixel inversion would require. Overall, I consider the project a success, and I'm just waiting for a case for the Inkplate to arrive now.

<h2>Contact</h2>
pdefrancisci57@gmail.com
https://www.linkedin.com/in/pdefrancisci57/

<h2>Known Issues</h2>
The refresh rate isn't very fast, but it's about what I expect for a weather radar, which doesn't race by. The dithering process I used used a 2x2 Bayer Matrix, this taught me a lot about how the different threshold impact the image, and I spent significant time tweaking my matrix. However, I'm not completely satisfied with the resulting image. I expect I could use a larger matrix to convey more detail, while still producing a monochrome bitmap with good performance. In particular the names of cities are pretty hard to read, after the matrix chews up compression artifacts near the text. Here's a rough sketch of the full design:
<img src="https://github.com/user-attachments/assets/d996062f-c86e-4fca-adf8-d04c19a9fcd1">



<h2>Future plans and updates</h2>
The server actually offers way more than I use one the Inkplate right now. I have a bunch of sensor data, and the whole flow is sketched out in the high level design. But I have interviews coming up so I have to drop this project to go do leetcode.

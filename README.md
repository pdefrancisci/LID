<h1>Life Improvement Device</h1>

<h2>Description</h2>
The life improvement device, currently, is an e-paper project. It is Soldered's Inkplate 6, which acts as a frontend for NWS radar images preprocessed by a raspberry pi. Most of the work thus far has involved finding tricks to overcome the limitations of e-ink displays. So I will get into that next.

<h2>[Inkplate 6](https://soldered.com/product/inkplate-6-6-e-paper-board/?attribute_pa_variant=with-e-paper)</h2>
The Inkplate 6 is a high end driver for e-paper displays. E-paper is a unique platform, it uses charged pigments to form an image, rather than brightness. As a result, the display is unique, and easy on the eyes, but also difficult to navigate, mainly due to it's high refresh rate, and limitation in color (the Inkplate 6 is black and white, offering greyscale in between).

My goal in this project was to produce a home assistant, which is a common use case for e-paper hobby projects. What made my project different is I wanted the display to handle animation. Typically, image transitions on e-paper require what's called pixel inversion, the screen turns white, then black, then you see your image. This eliminates all lingering charges, and your image appears. However, this is *very* slow, full refreshes such as these can take over a second, even on high end displays like the Inkplate.

<h2>Partial Refresh</h2>
To avoid this full refresh, I used a feature offered by some e-paper displays, known as partial refresh. You see, when only part of the screen has changed, it is unnecessary to update the entire display, and it takes time to do so. Thus, if you only update a subset of the display, you get faster, more subtle updates.

<h2>Dithering</h2>
I also previously alluded to the fact the Inkplate 6 supports only black and white. This includes a 3 bit greyscale. I opted to not use the greyscale, as a 1 bit monochrome bitmap would be more performant, and keep in mind, we're trying to smoothly imitate motion on an e-paper display. However, converting from 256/256/256 rgb in a .gif, straight to a bitmap was a non starter.

Contact

Include ways to reach you for support or inquiries.

Optional sections:

Screenshots: Add images of the project.

Known Issues: List any known problems.

Roadmap: Future plans and updates.

Acknowledgments: Credit contributors or inspirations.

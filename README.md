import digitalio
import board
import busio
from PIL import Image, ImageDraw, ImageChops, ImageFont
import PIL.ImageOps
from adafruit_rgb_display import ili9341 # disp 2.2
from adafruit_rgb_display import st7789  # disp 2.4
from adafruit_rgb_display import st7735  # disp 0.96

# First define some constants to allow easy resizing of shapes.
BORDER = 20
FONTSIZE = 36

# Configuration for CS and DC pins (these are PiTFT defaults):
# Display 0.96
cs_pin_d1 = digitalio.DigitalInOut(board.CE0)
dc_pin_d1 = digitalio.DigitalInOut(board.D24) #D24 to CS0
reset_pin_d1 = digitalio.DigitalInOut(board.D22) #D22 to CS0

# Other Display 0.96
cs_pin_d3 = digitalio.DigitalInOut(board.D27)
dc_pin_d3 = digitalio.DigitalInOut(board.D17) #D24 to CS0
reset_pin_d3 = digitalio.DigitalInOut(board.D13) #D22 to CS0

# cs_pin_d11 = digitalio.DigitalInOut(board.D8)
# cs_pin_d11 = digitalio.Direction.OUTPUT
# cs_pin_d11 = True

# Display 2.2
cs_pin_d2 = digitalio.DigitalInOut(board.CE1)
dc_pin_d2 = digitalio.DigitalInOut(board.D12)#D12 to CS1
reset_pin_d2 = digitalio.DigitalInOut(board.D16)#D16 CS1
# Config for display baudrate (default max is 24mhz):

# cs_pin1.value = True
BAUDRATE = 24000000

# Setup SPI bus using hardware SPI:
spi = board.SPI()

disp_96 = st7735.ST7735R(spi,
    x_offset=2, y_offset=3,
    bgr=True, #inverts color for ST7735R and ST7789
    rotation=90,
    cs=cs_pin_d1,
    dc=dc_pin_d1,
    rst=reset_pin_d1,
    baudrate=BAUDRATE,
)

secon_96 = st7735.ST7735R(spi,
    x_offset=2, y_offset=3,
    bgr=True, #inverts color for ST7735R and ST7789
    rotation=90,
    cs=cs_pin_d3,
    dc=dc_pin_d3,
    rst=reset_pin_d3,
    baudrate=BAUDRATE,
)


disp_22 = ili9341.ILI9341(
    spi,
    rotation=180, 
    cs=cs_pin_d2,
    dc=dc_pin_d2,
    rst=reset_pin_d2,
    baudrate=BAUDRATE,
)


# Create blank image for drawing.
# Make sure to create image with mode 'RGB' for full color.
if disp_22.rotation % 180 == 90:
    height = disp_22.width  # we swap height/width to rotate it to landscape!
    width = disp_22.height
else:
    width = disp_22.width  # we swap height/width to rotate it to landscape!
    height = disp_22.height
image = Image.new("RGB", (width, height))

if disp_96.rotation % 180 == 90:
    width_96 = disp_96.height
    height_96 = disp_96.width
else:
    width_96 = disp_96.width
    height_96 = disp_96.height
image_96 = Image.new("RGB", (width_96, height_96))

if secon_96.rotation % 180 == 90:
    width_96 = secon_96.height
    height_96 = secon_96.width
else:
    width_96 = secon_96.width
    height_96 = secon_96.height
other_img = Image.new("RGB", (width_96, height_96))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)
draw_96 = ImageDraw.Draw(image_96)
other_draw_96 = ImageDraw.Draw(other_img)

# Draw a black filled box to clear the image.
draw.rectangle((0, 0, width, height), outline=0, fill=(0, 0, 0))
disp_22.image(image)

image = Image.open("charmander.jpg")
#image = PIL.ImageChops.invert(PIL.Image.open("charmander.jpg"))

# Scale the image to the smaller screen dimension
image_ratio = image.width / image.height
screen_ratio = width / height
if screen_ratio < image_ratio:
    scaled_width = image.width * height // image.height
    scaled_height = height
else:
    scaled_width = width
    scaled_height = image.height * width // image.width
image = image.resize((width, height), Image.BICUBIC)

# Crop and center the image
x = scaled_width // 2 - width // 2
y = scaled_height // 2 - height // 2
image = image.crop((x, y, x + width, y + height))

# Display image.
disp_22.image(image)

# Draw a green filled box as the background
draw_96.rectangle((0, 0, width_96, height_96), fill=(240, 128, 48))
print(image_96)
disp_96.image(image_96)

# Load a TTF Font
font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", FONTSIZE)

# Draw Some Text
text = "Fogo"
(font_width, font_height) = font.getsize(text)
draw_96.text(
    (width_96 // 2 - font_width // 2, height_96 // 2 - font_height // 2),
    text,
    font=font,
    fill=(0, 0, 0),
)
disp_96.image(image_96)

# Draw a green filled box as the background
other_draw_96.rectangle((0, 0, width_96, height_96), fill=(240, 128, 48))

text = "TESTE"
(font_width, font_height) = font.getsize(text)
other_draw_96.text(
    (width_96 // 2 - font_width // 2, height_96 // 2 - font_height // 2),
    text,
    font=font,
    fill=(0, 0, 0),
)
secon_96.image(image_96)





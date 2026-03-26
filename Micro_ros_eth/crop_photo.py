from PIL import Image

# Load CV image
img_path = r"C:\Users\minis\.gemini\antigravity\brain\3ded665d-1a14-4d6b-aabd-ad6d2e1b292e\media__1774432150069.png"
img = Image.open(img_path)

w, h = img.size
print(f"Image size: {w}x{h}")

# The photo is roughly in the top right corner.
# Crop coordinates: (left, upper, right, lower)
left = int(w * 0.74)
upper = int(h * 0.02)
right = int(w * 0.98)
lower = int(h * 0.17)

crop_img = img.crop((left, upper, right, lower))

# Make it square
cw, ch = crop_img.size
min_dim = min(cw, ch)
center_x, center_y = cw // 2, ch // 2
half = min_dim // 2

square_crop = crop_img.crop((
    center_x - half,
    center_y - half,
    center_x + half,
    center_y + half
))

# Resize to 400x400
final_img = square_crop.resize((400, 400), Image.Resampling.LANCZOS)
final_img.save(r"C:\Users\minis\portfolio\profile-photo.png")
print("Saved profile-photo.png")

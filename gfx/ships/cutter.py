import os
from PIL import Image

size = 20
def cut(name):
    try:
        img = Image.open(f"{name}_full.png")
        width = img.size[0]
        slices = width//size
        
        out = []
        for slice in range(slices):
            left = slice * size
            bbox = (left, 0, left+size, size)
            
            working_slice = img.crop(bbox)
            print(f"Saving part {slice+1} of {name}")
            path = f"{name}_part_{slice+1}.png"
            out.append(path)
            working_slice.save(path)
        return "\n".join(out)
    except Exception as e:
        print("Exception occured:", e)
        return ""

def main():
    names = [
        "Carrier",
        "Battleship",
        "Cruiser",
        "Submarine",
        "Destroyer",
    ]
    out = []
    for name in names:
        out.append(cut(name))
    print("\n".join(out))

if __name__ == '__main__':
    main()
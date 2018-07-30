import os

def main():
    names = [
        "Carrier",
        "Battleship",
        "Cruiser",
        "Submarine",
        "Destroyer",
    ]
    for name in names:
        for file in os.listdir(name):
            os.rename(f"{name}/{file}", f"{name}_{file}")

if __name__ == '__main__':
    main()
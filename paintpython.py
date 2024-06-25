
import tkinter as tk
from tkinter import filedialog
from PIL import Image, ImageDraw, ImageTk
#pip install pillow
class PaintApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Paint Application")
        self.root.configure(bg='yellow')

        self.canvas_width = 800
        self.canvas_height = 600

        self.canvas = tk.Canvas(root, bg='white', width=self.canvas_width, height=self.canvas_height)
        self.canvas.pack(padx=10, pady=10)

        self.filename_var = tk.StringVar(value="new.bmp")

        self.entry_filename = tk.Entry(root, textvariable=self.filename_var)
        self.entry_filename.pack(padx=10, pady=5)

        self.button_new = tk.Button(root, text="New", command=self.new_image)
        self.button_new.pack(side=tk.LEFT, padx=10, pady=5)

        self.button_load = tk.Button(root, text="Load", command=self.load_image)
        self.button_load.pack(side=tk.LEFT, padx=10, pady=5)

        self.button_save = tk.Button(root, text="Save", command=self.save_image)
        self.button_save.pack(side=tk.LEFT, padx=10, pady=5)

        self.image = Image.new("RGB", (self.canvas_width, self.canvas_height), "white")
        self.draw = ImageDraw.Draw(self.image)
        self.canvas_image = ImageTk.PhotoImage(self.image)
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.canvas_image)

        self.canvas.bind("<B1-Motion>", self.paint)

    def new_image(self):
        self.image = Image.new("RGB", (self.canvas_width, self.canvas_height), "white")
        self.draw = ImageDraw.Draw(self.image)
        self.canvas_image = ImageTk.PhotoImage(self.image)
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.canvas_image)

    def load_image(self):
        filename = self.filename_var.get()
        try:
            self.image = Image.open(filename)
            self.canvas_image = ImageTk.PhotoImage(self.image)
            self.canvas.create_image(0, 0, anchor=tk.NW, image=self.canvas_image)
            self.draw = ImageDraw.Draw(self.image)
        except Exception as e:
            tk.messagebox.showerror("Error", f"Failed to load image: {str(e)}")

    def save_image(self):
        filename = self.filename_var.get()
        try:
            self.image.save(filename)
            tk.messagebox.showinfo("Success", "Image saved successfully.")
        except Exception as e:
            tk.messagebox.showerror("Error", f"Failed to save image: {str(e)}")

    def paint(self, event):
        x, y = event.x, event.y
        self.draw.ellipse([x-2, y-2, x+2, y+2], fill="black", outline="black")
        self.canvas_image = ImageTk.PhotoImage(self.image)
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.canvas_image)

if __name__ == "__main__":
    root = tk.Tk()
    app = PaintApp(root)
    root.mainloop()

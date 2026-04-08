from PIL import Image
import os

# 打开图片
image_path = 'Tiles/121.jpg'
img = Image.open(image_path)

# 获取图片尺寸
width, height = img.size
print(f"原始图片尺寸: {width}x{height}")

# 定义裁剪区域 (左, 上, 右, 下)
# 由于图片中草坪是主要部分，我将裁剪掉周围的沙地
left = 50
upper = 50
right = width - 50
lower = height - 50

# 裁剪图片
cropped_img = img.crop((left, upper, right, lower))

# 保存裁剪后的图片
output_path = 'Tiles/121.png'
cropped_img.save(output_path)

print(f"裁剪后的图片已保存为: {output_path}")
print(f"裁剪后图片尺寸: {cropped_img.size}")

from datasets import load_dataset

N = 1000
ds_peop = load_dataset("beurkinger/autotrain-data-human-action-recognition")['train'].shuffle(seed=42)
ds_dogs = load_dataset("chandocchi/dog-dataset")['train'].shuffle(seed=42) 

save_dir = 'predict'
for i in range(int(N*0.2)):
    img = ds_peop[i]["image"]
    img.save(f"{save_dir}\\image_{i}P.jpg")
for i in range(int(N*0.2)):
    img = ds_dogs[i]["image"]
    img.save(f"{save_dir}\\image_{i}D.jpg")
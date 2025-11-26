import subprocess

# data = []
height = 3200
for num_thread in [1, 2, 4, 8, 16, 32]:
    for width in [2, 4, 8, 16, 32]:
        # print(num_thread, width)
        res = subprocess.run(['./qr_demo', '-p', str(num_thread), '-w', str(width), '-h', str(height)], 
                        capture_output=True,
                        text=True,)
        # print(res.stdout)
        res = int(res.stdout.splitlines()[-1].split(':')[1].split()[0])
        print({
            'height': height,
            'width': width,
            'thread': num_thread,
            'time': res,
        })

# print(data)
# pd.DataFrame(data).to_csv("mp_bench.csv", index=False)
        
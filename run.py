import subprocess

# data = []
# height = 3200
for height in [2**i for i in range(2, 14+1)]:
    for width in [2, 4]:
        # print(num_thread, width)
        res = subprocess.run(['./qr_demo', '-w', str(width), '-h', str(height)], 
                        capture_output=True,
                        text=True,)
        # print(res.stdout)
        # print(res.stderr)
        res = int(res.stdout.splitlines()[-1].split(':')[1].split()[0])
        print({
            'height': height,
            'width': width,
            'time': res,
        })

# print(data)
# pd.DataFrame(data).to_csv("mp_bench.csv", index=False)
        
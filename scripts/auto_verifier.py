import subprocess
import os
from shutil import copyfile
import argparse
import patoolib
import re
from tqdm import tqdm

string = "AB12345678 CD98765432 EF87654321"
pattern = r"[A-Z]{2}\d{8}"

result_file = "result.csv"
log_dir = "log"

PATH_TO_LC3 = "/Users/cydia2001/git_repo/lc3tools/build/bin/labtest"

def get_filelist(dir):
    filelist = []
    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith(".zip") or file.endswith(".rar") or file.endswith('.7z'):
                filelist.append(os.path.join(root, file))
    print(filelist)
    return filelist

def random_path_generator():
    import random
    import string
    return ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
  
def unzip_file(file_path, base_path):
    # stu_id = os.path.basename(file_path).split('_ ')[0]
    stu_id = re.findall(pattern, file_path)[0]
    if not os.path.exists(os.path.join(base_path, stu_id)):
        patoolib.extract_archive(file_path, outdir=os.path.join(base_path, stu_id))
    return stu_id

def find_endwith(file_path,end_with):
    for root, dirs, files in os.walk(file_path):
        for file in files:
            if file.endswith(end_with):
                return os.path.join(root, file)
    return None

def generate_test_list(base_path, lab_id = 1):
  filelist = get_filelist(base_path)
  stu_file_dir = {}
  submission_end = "bin"
  if lab_id == 2:
      submission_end = "asm"
  for f in filelist:
    stu_id = unzip_file(f, base_path)
    full_path_to_submission = os.path.join(base_path, stu_id)
    stu_file_dir[stu_id] = find_endwith(full_path_to_submission, submission_end)
  return stu_file_dir

def parse_args():
  parser = argparse.ArgumentParser()
  parser.add_argument("--stu_path", type=str, required=True)
  parser.add_argument("--lab", type=int, default=1)
  parser.add_argument("--test-input", type=str, required=True)
  args = parser.parse_args()
  return args

def main(args):
    file_list = generate_test_list(args.stu_path, args.lab)
    if not os.path.exists("./tmp"):
        os.mkdir("tmp")
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)
    file_extension = "bin"
    if args.lab == 2:
        file_extension = "asm"
    for stu_id, file_path in file_list.items():
        copyfile(file_path, os.path.join("./tmp", stu_id + f".{file_extension}"))
        file_list[stu_id] = os.path.join("./tmp", stu_id + f".{file_extension}")
    print(file_list)
    
    with open(result_file, "w") as f:
        f.write("Student ID, Result, Score\n")
        pbar = tqdm(file_list.items())
        for stu_id, file_path in pbar:
            # print("Running test for student: {}".format(stu_id))
            pbar.set_description("Running test for student: {}".format(stu_id))
            result = subprocess.run(
                [PATH_TO_LC3, f"--lab={args.lab}", f"--test-input={args.test_input}", "--print-level=3", file_path], 
                capture_output=True,
                text=True
            )
            stdout = result.stdout.strip()
            outlines = stdout.split("\n")
            # print(outlines)
            outlines = [line for line in outlines if "Test case" in line or "Passed" in line]
            count = int(outlines[-1].split()[1])
            total = int(outlines[-1].split()[-3])
            score = count / total * 2.5
            if f"Student ID: {stu_id[-1]}" not in outlines[0]: # student ID not match
                score -= total * 0.05   
            score = max(0, score)
            f.write(f"{stu_id}, {count}/{total}, {score}\n")
            with open(os.path.join(log_dir, f"{stu_id}.log"), "w") as log:
                log.write("\n".join(outlines))
  
if __name__ == "__main__":
    args = parse_args()
    main(args)
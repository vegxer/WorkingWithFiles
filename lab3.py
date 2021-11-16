import os
import shutil
import stat
import sys


def is_hidden(path):
	if not os.path.exists(path):
		raise Exception("File not found")
		
	if sys.platform == "win32":
		return os.stat(path).st_file_attributes & stat.FILE_ATTRIBUTE_HIDDEN != 0
	elif sys.platform == "linux2" or sys.platform == "linux":
		return os.path.split(path)[1][0] == '.'
	else:
		raise Exception("Incorrect platform")
        
def get_size(path):
    if os.path.isfile(path):
        return os.path.getsize(path)
    
    size = 0
    for item in os.listdir(path):
        item_name = path + '/' + item
        if os.path.isfile(item_name):
            size += os.path.getsize(item_name)
        else:
            size += get_size(item_name)
            
    return size

def print_files(path):
	for file in os.listdir(path):
		print(file, " (", get_size(path + "/" + file) / 1024.0, " KB)")
		
def delete_item(path):
	if os.path.isfile(path):
		os.remove(path)
	else:
		shutil.rmtree(path)

		
if __name__ == "__main__":
	active = True
	while (active):
		print("1 - Print directory items list")
		print("2 - Create new file")
		print("3 - Copy file")
		print("4 - Delete file/directory")
		print("5 - Move/rename directory")
		print("6 - Set readonly attribute")
		print("7 - Remove readonly attribute")
		print("8 - Check if file is hidden")
		print("9 - Quit program")
		choice = input("Your choice: ")
		try:
			if choice == '1':
				print_files(input("Input directory path: "))
			elif choice == '2':
				open(input("Input file path: "), 'a').close()
			elif choice == '3':
				shutil.copyfile(input("Input source path: "), input("Input destination path: "))
			elif choice == '4':
				delete_item(input("Input path: "))
			elif choice == '5':
				shutil.move(input("Input source path: "), input("Input destination path: "))
			elif choice == '6':
				os.chmod(input("Input path: "), stat.S_IREAD)
			elif choice == '7':
				os.chmod(input("Input path: "), stat.S_IWRITE | stat.S_IREAD)
			elif choice == '8':
				if is_hidden(input("Input path: ")):
					print("File is hidden")
				else:
					print("File in not hidden")
			elif choice == '9':
				active = False
			else:
				raise Exception("Incorrect input")
				
			print("Successfully")
		except:
			print("Exception occurred while executing")
		
		print('\n')
		

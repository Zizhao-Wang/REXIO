import os
import random


def generate_unique_random_numbers(min_value, max_value, update_num_samples, delete_num_samples):
    # First part: sequentially generate numbers from min_value to update_num_samples
    # seq_numbers = list(range(min_value, min_value + update_num_samples))

    # Second part: generate numbers from update_num_samples to max_value using step
    # step = (max_value - min_value - update_num_samples + 1) // delete_num_samples
    # random_numbers = list(range(min_value + update_num_samples, max_value+1, step))

    random_numbers = random.sample(range(min_value, max_value + 1), update_num_samples+delete_num_samples)
    return  random_numbers
    # return seq_numbers + random_numbers[:delete_num_samples]

def save_to_file(data, filename):
    with open(filename, 'w') as file:
        for number in data:
            file.write(str(number) + '\n')

def remove_file_if_exists(filename):
    if os.path.isfile(filename):
        os.remove(filename)

min_value = 1

# Adjust the range as per your requirement
for key in [8, 16]:
    for val in [8, 16, 32, 64, 128, 256,1024,2048]:
        max_value = (100000000 * 16) // (key + val)
        total_num_samples = max_value // 2 + max_value // 5
        print(total_num_samples)
        update_num_samples = max_value // 2
        delete_num_samples = max_value // 5
        print(update_num_samples)
        print(delete_num_samples)

        random_numbers = generate_unique_random_numbers(min_value, max_value, update_num_samples, delete_num_samples)

        # For update
        filename = f'/home/TiOCS/src/data/update_{key}_{val}.txt'
        remove_file_if_exists(filename)
        save_to_file(random_numbers[:update_num_samples], filename)

        # For delete
        filename = f'/home/TiOCS/src/data/delete_{key}_{val}.txt'
        remove_file_if_exists(filename)
        save_to_file(random_numbers[update_num_samples:], filename)

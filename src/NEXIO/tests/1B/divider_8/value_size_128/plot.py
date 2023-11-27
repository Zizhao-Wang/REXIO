# import matplotlib.pyplot as plt
# import numpy as np

# # Load data from the file
# data = np.loadtxt('zipf_data.txt', dtype=np.uint64)

# # Calculate the number of occurrences of each value in the data
# values, counts = np.unique(data, return_counts=True)

# # Sort the counts in descending order to align with Zipf's law ranking
# sorted_indices = np.argsort(-counts)
# sorted_counts = counts[sorted_indices]
# sorted_values = values[sorted_indices]

# # Create the log-log plot
# plt.figure(figsize=(12, 7))
# plt.loglog(sorted_values, sorted_counts, 'o-', color='blue')  # Log-log plot
# plt.title('Log-Log Distribution of Zipf Data')
# plt.xlabel('Rank of the Data Value')
# plt.ylabel('Frequency')
# plt.grid(True, which="both", ls="--")

# # Save the figure to a file
# plt.savefig('zipf_distribution_loglog.png')  # Update with your preferred path
# plt.show()  # If you want to also display the plot
# plt.close()



import matplotlib.pyplot as plt
import numpy as np

# Load data from the file
data = np.loadtxt('zipf_data.txt', dtype=np.uint64)

# Calculate the number of occurrences of each value in the data
values, counts = np.unique(data, return_counts=True)


# Sort the counts in descending order and get the indices
sorted_indices = np.argsort(-counts)

# Get the top 10 values and their respective counts
top_values = values[sorted_indices][:10]
top_counts = counts[sorted_indices][:10]

# Print the top 10 values and their frequencies
for value, count in zip(top_values, top_counts):
    print(f"Value: {value}, Frequency: {count}")

# Create the plot
plt.figure(figsize=(12, 7))
plt.plot(values, counts, 'o-', color='blue')  # Regular plot with circle markers
plt.title('Frequency Distribution of Zipf Data')
plt.xlabel('Data Value')
plt.ylabel('Frequency')
plt.grid(True)

# Save the figure to a file
plt.savefig('zipf_distribution_plot.png')  # Update with your preferred path
# plt.show()  # If you want to also display the plot
plt.close()


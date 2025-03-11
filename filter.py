# ------- User defined parameters -------

# Lowest frequency to print words of
freqlower = 1.3

# Highest frequency to print words of
frequpper = 2.5

# ---------------------------------------

from wordfreq import zipf_frequency

# Open raw_words file to get all the words
# Pass them through the frequency filter
# Output to filtered_words file

input_file = open("raw_words", "r")
output_file = open("filtered_words", "w")

while(True):
    # Get word from file
    word = input_file.readline().strip()
    
    # Break if no input
    if len(word) == 0:
        break;

    # Get frequency
    freq = zipf_frequency(word, "en");

    # Continue if outside of range
    if freq < freqlower or freq >= frequpper:
        continue

    # Write out all words to the output file
    output_file.write(word)
    output_file.write("\n")

# Close all file handles
input_file.close();
output_file.close();
import json
import re


def convert_to_json(file_path: str) -> str:
    """
    Convert a sensor measurements text file to a JSON file.
    :param file_path:
    :return: The path to the created JSON file.
    """

    # load the data from the file
    with open(file_path, "r") as file:
        data = file.readlines()

    # remove headers, empty lines, and linebreaks
    data = [line[:-1] for line in data if line[0:4] == "time"]
    result = []
    for line in data:
        time_match = re.search(r"time: \[([0-9\.\-eE]+)\]", line)
        imu_match = re.search(r"imu: \[([0-9\.\,\-\seE]+)\]", line)
        state_match = re.search(r"state: \[([0-9\.\,\-\seE]+)\]", line)
        if time_match and imu_match and state_match:
            time_val = float(time_match.group(1))
            imu_vals = [float(x) for x in imu_match.group(1).split(",")]
            state_vals = [float(x) for x in state_match.group(1).split(",")]
            result.append({"time": time_val, "imu": imu_vals, "state": state_vals})

    # save as json
    json_file_path = file_path.replace(".txt", ".json")
    with open(json_file_path, "w") as json_file:
        json.dump(result, json_file, indent=4)

    return json_file_path

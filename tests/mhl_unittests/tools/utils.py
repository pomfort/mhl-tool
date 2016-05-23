def dicts_from_table(keys, array_of_values):
    dicts = []
    for values in array_of_values:
        dicts.append({key: value for key, value in zip(keys, values)})

    return dicts

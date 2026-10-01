
def find(arr, num):
    left = 0
    right = len(arr) - 1
    iterations = 0

    while left <= right:
        mid = left + (right - left) // 2
        iterations += 1

        if arr[mid] == num:
            return iterations
        if arr[mid] < num:
            left = mid + 1
        else:
            right = mid - 1

    return -1

arr = list(range(1, 37))


iters = [find(arr, i) for i in range(1, 37)]
print(list(range(1, 37)))
print(iters)

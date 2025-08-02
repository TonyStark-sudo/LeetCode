def select_sort(nums: list):
    n = len(nums)
    for i in range(0, n - 1):
        for j in range(i, n):
            if nums[i] > nums[j]:
                nums[i], nums[j] = nums[j], nums[i]
def bucket_sort(nums: list):
    minV = min(nums)
    maxV = max(nums)
    bucket_num = 3
    buckets = [[], [], []]
    bucket_range = (maxV - minV) / bucket_num

    for num in nums:
        buck_index = (num - minV) / bucket_range
        print(f"buck_index is {buck_index}")
        buck_index = int(buck_index)

        if buck_index >= bucket_num:
            buck_index = bucket_num - 1
        buckets[buck_index].append(num)
    
    result = []
    for bucket in buckets:
        select_sort(bucket)
        result += bucket
    return result
if __name__ == "__main__":
    nums = [10, 3, 1, 8, 4,  7, 6]
    result = bucket_sort(nums)
    print(result)
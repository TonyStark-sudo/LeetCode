
def merge_sort(nums):
    """
    归并排序主函数
    """
    if len(nums) <= 1:
        return nums
    
    # 分治：将数组分成两半
    mid = len(nums) // 2
    left = merge_sort(nums[:mid])   # 递归排序左半部分
    right = merge_sort(nums[mid:])  # 递归排序右半部分
    
    # 合并两个已排序的数组
    return merge(left, right)

def merge(left, right):
    """
    合并两个已排序的数组
    """
    result = []
    i = j = 0
    
    # 比较两个数组的元素，将较小的放入结果数组
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1
    
    # 将剩余元素添加到结果数组
    result.extend(left[i:])   # 左数组剩余元素
    result.extend(right[j:])  # 右数组剩余元素
    
    return result

def merge_sort_inplace(nums, left=0, right=None):
    """
    原地归并排序（更节省空间）
    """
    if right is None:
        right = len(nums) - 1
    
    if left < right:
        mid = (left + right) // 2
        
        # 递归排序左右两部分
        merge_sort_inplace(nums, left, mid)
        merge_sort_inplace(nums, mid + 1, right)
        
        # 合并两部分
        merge_inplace(nums, left, mid, right)

def merge_inplace(nums, left, mid, right):
    """
    原地合并两个已排序的子数组
    """
    # 创建临时数组保存左半部分
    left_arr = nums[left:mid + 1]
    
    i = 0  # 左数组指针
    j = mid + 1  # 右数组指针
    k = left  # 原数组指针
    
    # 合并过程
    while i < len(left_arr) and j <= right:
        if left_arr[i] <= nums[j]:
            nums[k] = left_arr[i]
            i += 1
        else:
            nums[k] = nums[j]
            j += 1
        k += 1
    
    # 复制左数组剩余元素
    while i < len(left_arr):
        nums[k] = left_arr[i]
        i += 1
        k += 1

if __name__ == "__main__":
    nums = [10, 3, 1, 8, 4, 7, 6]
    print(f"原始数组: {nums}")
    
    # 方法1：返回新数组的归并排序
    sorted_nums = merge_sort(nums.copy())
    print(f"归并排序结果: {sorted_nums}")
    
    # 方法2：原地归并排序
    nums_copy = nums.copy()
    merge_sort_inplace(nums_copy)
    print(f"原地归并排序结果: {nums_copy}")
    
    # 展示分治过程
    print("\n=== 分治过程演示 ===")
    def merge_sort_verbose(nums, depth=0):
        indent = "  " * depth
        print(f"{indent}分治: {nums}")
        
        if len(nums) <= 1:
            print(f"{indent}返回: {nums}")
            return nums
        
        mid = len(nums) // 2
        print(f"{indent}分割点: {mid}")
        
        left = merge_sort_verbose(nums[:mid], depth + 1)
        right = merge_sort_verbose(nums[mid:], depth + 1)
        
        result = merge(left, right)
        print(f"{indent}合并: {left} + {right} = {result}")
        return result
    
    merge_sort_verbose([10, 3, 1, 8])

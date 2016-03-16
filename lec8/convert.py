PDE_BASE = 0xd80

def getmem(filename):
	f = file(filename)
	mem = []
	for line in f.readlines():
		a = line.strip().split(' ')
		for x in a[2:]:
			mem.append(int(x, 16))
	f.close()
	return mem

def getcontent(base, index, mem):
	return mem[base + index]

def convert(va, mem, disk):
	print 'Virtual Address 0x%x:' %va
	pde_index = (va >> 10) & 0x1f
	pte_index = (va >> 5) & 0x1f
	offset = va & 0x1f
	pde_content = getcontent(PDE_BASE, pde_index, mem)
	pde_valid = pde_content >> 7
	pt = pde_content & 0x7f

	print '\t--> pde index: 0x%x pde contents: (valid 0x%x, pt 0x%x)' % (pde_index, pde_valid, pt)

	if pde_valid == 0:
		print '\t\t--> Fault (page directory entry not valid)'
		return
	
	pte_base = pt << 5
	pte_content = getcontent(pte_base, pte_index, mem)
	pte_valid = pte_content >> 7
	pfn = pte_content & 0x7f

	print '\t\t--> pte index: 0x%x pte contents: (valid 0x%x, pfn 0x%x)' % (pte_index, pte_valid, pfn)

	if pte_valid == 0:
		if pfn == 0x7f:
			print '\t\t\t--> Fault (page table entry not valid)'
			return
		disk_base = pfn << 5
		disk_content = getcontent(disk_base, offset, disk)
		print '\t\t\t--> To Disk Sector Address 0x%x --> Value: 0x%x' % (disk_base + offset, disk_content)
		return

	mem_base = pfn << 5
	mem_content = getcontent(mem_base, offset, mem)
	print '\t\t\t--> To Physical Address 0x%x --> Value: 0x%x' % (mem_base + offset, mem_content)


if __name__ == '__main__':
	mem = getmem('mem.txt')
	disk = getmem('disk.txt')
	f = file('input.txt')
	for line in f.readlines():
		va = int(line, 16)
		convert(va, mem, disk)

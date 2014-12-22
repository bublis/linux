#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>

char e1000_driver_name[] = "e1000";
#define INTEL_E1000_ETHERNET_DEVICE(device_id) {\
	PCI_DEVICE(PCI_VENDOR_ID_INTEL, device_id)}

static const struct pci_device_id e1000_pci_tbl[] = {
	INTEL_E1000_ETHERNET_DEVICE(0x100E),
	{0,}
};

MODULE_DEVICE_TABLE(pci, e1000_pci_tbl);


static int bars;

static int probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	u8 __iomem *ioaddr;
	unsigned int reg;
	
	int err;
	/* создает маску BAR из типов ресурсов */
	bars = pci_select_bars(pdev, IORESOURCE_MEM | IORESOURCE_IO);
	/* инициализация устройства */
	err = pci_enable_device(pdev);
	if (err)
		return err;
	/* резервирует регионы памяти */
	err = pci_request_selected_regions(pdev, bars, e1000_driver_name);
	if (err)
		goto err_pci_reg;
	/* отображает память устройства в виртуальной памяти */
	ioaddr = pci_ioremap_bar(pdev, 0);	
	if (!ioaddr)
		goto err_ioremap;
	/* чтение 4 байт из BAR0 со смещением 0x0038 */
	reg = readl(ioaddr + 0x0038);		
	printk("%x\n", reg);

	return 0;

err_ioremap:
	/* освобождает регионы памяти */
	pci_release_selected_regions(pdev, bars);
err_pci_reg:
	pci_disable_device(pdev);
	return err;
}

static void remove(struct pci_dev *pdev)
{
	/* освобождение памяти которое занимало устройство и отключение устройства */
	pci_release_selected_regions(pdev, bars);
	pci_disable_device(pdev);
}

/*
 * Структура PCI драйвера
 * name		имя драйвера
 * id_table	таблица vendor_id/device_id. при появлении устройства на шине произойдет вызов probe
 * probe	указатель на функцию remove
 * remove	указатель на функцию probe
 */
static struct pci_driver pci_driver = { 
	.name = "pci_skel",	
	.id_table = e1000_pci_tbl,
	.probe = probe,		
	.remove = remove,	
};

static int __init pci_skel_init(void)
{
	printk("itstalled e1000_main\n");
	/* регистрирует драйвер в ядре PCI */
	return pci_register_driver(&pci_driver);
}

static void __exit pci_skel_exit(void)
{
	printk("removed e1000_main\n");
	pci_unregister_driver(&pci_driver); //выгружает драйвер в ядре PCI
}
/* говорит модулю какую лицензию использует модуль, без него модуль напишит предупреждение */
MODULE_LICENSE("GPL");
/* функции используют пространство (макросы) ядра для установки и удаления модуля */
module_init(pci_skel_init);
module_exit(pci_skel_exit);

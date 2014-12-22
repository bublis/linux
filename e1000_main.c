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

	bars = pci_select_bars(pdev, IORESOURCE_MEM | IORESOURCE_IO);   //
	err = pci_enable_device(pdev);					//включение устройства
	if (err)
		return err;

	err = pci_request_selected_regions(pdev, bars, e1000_driver_name); //резервирует регионы памяти
	if (err)
		goto err_pci_reg;

	ioaddr = pci_ioremap_bar(pdev, 0);				//отображает память устройства в физической памяти
	if (!ioaddr)
		goto err_ioremap;
	reg = readl(ioaddr + 0x0038);
	printk("%x\n", reg);

	return 0;

err_ioremap:
	pci_release_selected_regions(pdev, bars);  	//освобождает регионы памяти
err_pci_reg:
	pci_disable_device(pdev);
	return err;
}

static void remove(struct pci_dev *pdev)
{
	pci_release_selected_regions(pdev, bars); //освобождение памяти которое занимало устройство
	pci_disable_device(pdev);		  //отключение устройства
}

static struct pci_driver pci_driver = { //Структура PCI драйвера
	.name = "pci_skel",		//имя драйвера
	.id_table = e1000_pci_tbl,	//заносит vendor_id и device_id в таблицу. при появлении устройства на шине произойдет вызов probe
	.probe = probe,			//указатель на функцию probe
	.remove = remove,		//указатель на функцию remove
};

static int __init pci_skel_init(void)
{
	printk("itstalled e1000_main\n");
	return pci_register_driver(&pci_driver); //регистрирует драйвер в ядре PCI
}

static void __exit pci_skel_exit(void)
{
	printk("removed e1000_main\n");
	pci_unregister_driver(&pci_driver); //выгружает драйвер в ядре PCI
}

MODULE_LICENSE("GPL"); //говорит модулю какую лицензию использует модуль, без него модуль напишит предупреждение

module_init(pci_skel_init); //функции используют пространство (макросы) ядра
module_exit(pci_skel_exit); //для установки и удаления модуля

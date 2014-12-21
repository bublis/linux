 
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
	u8 __iomem *hw_addr;
	unsigned int mac[6];
	
	int err, i;

	bars = pci_select_bars(pdev, IORESOURCE_MEM | IORESOURCE_IO);
	err = pci_enable_device(pdev);
	if (err)
		return err;

	err = pci_request_selected_regions(pdev, bars, e1000_driver_name);
	if (err)
		goto err_pci_reg;

	hw_addr = pci_ioremap_bar(pdev, 0);
	if (!hw_addr)
		goto err_ioremap;
	for (i = 0; i < 6; i++)
		mac[i] = readb(hw_addr);

	printk("mac=");
	for (i = 0; i < 6; i++)
		printk("%x:", mac[i]);

	return 0;

err_ioremap:
	pci_release_selected_regions(pdev, bars);
err_pci_reg:
	pci_disable_device(pdev);
	return err;
}

static void remove(struct pci_dev *pdev)
{
	/* clean up any allocated resources and stuff here.
	 * like call release_region();
	 */
	pci_release_selected_regions(pdev, bars);
	pci_disable_device(pdev);
}

static struct pci_driver pci_driver = {
	.name = "pci_skel",
	.id_table = e1000_pci_tbl,
	.probe = probe,
	.remove = remove,
};

static int __init pci_skel_init(void)
{
	printk("itstalled e100_main\n");
	return pci_register_driver(&pci_driver);
}

static void __exit pci_skel_exit(void)
{
	printk("removed e100_main\n");
	pci_unregister_driver(&pci_driver);
}

MODULE_LICENSE("GPL");

module_init(pci_skel_init);
module_exit(pci_skel_exit); 
